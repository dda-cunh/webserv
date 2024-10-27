#include "../../includes/classes/CGIHandler.hpp"

CGIHandler::CGIHandler(Response& response, const std::string& cgiPath)
    : _response(response), _cgiPath(cgiPath) {
    setEnvironmentVariables();
    createPipes();
}

CGIHandler::~CGIHandler() {
    close(_inputPipe[0]);
    close(_inputPipe[1]);
    close(_outputPipe[0]);
    close(_outputPipe[1]);
}

void CGIHandler::setEnvironmentVariables() {
    const Request& request = _response.getRequest();
    _envVars.push_back("REQUEST_METHOD=" + Http::methodToString(request.method()));
    _envVars.push_back("PATH_INFO=" + _response.getCGIMatch().getPathInfo());
    _envVars.push_back("SCRIPT_FILENAME=" + _cgiPath);
    _envVars.push_back("UPLOAD_DIR=" + _response.getLocationMatch()->getUploadPath());

    if (request.method() == Http::M_POST) {
        _envVars.push_back("CONTENT_TYPE=" + request.header("Content-Type"));
        _envVars.push_back("CONTENT_LENGTH=" + request.header("Content-Length"));
    }

    Utils::log("CGI process env variables::", Utils::LOG_INFO);
    for (size_t i = 0; i < _envVars.size(); ++i)
        std::cout << "\t" << _envVars[i] << std::endl;
}

void CGIHandler::createPipes() {
    if (pipe(_inputPipe) == -1 || pipe(_outputPipe) == -1)
        throw ExceptionMaker("Pipe creation failed: " + std::string(strerror(errno)));
}

void CGIHandler::handleChildProcess() {
    close(_inputPipe[1]);
    close(_outputPipe[0]);

    std::string execDir = Utils::concatenatePaths(_response.getLocationMatch()->getRootDir().c_str(), _response.getCGIMatch().getBasePath().c_str(), NULL);
    std::string execFile = _response.getCGIMatch().getScriptName();

    dup2(_inputPipe[0], STDIN_FILENO);
    dup2(_outputPipe[1], STDOUT_FILENO);
    dup2(_outputPipe[1], STDERR_FILENO);

    close(_inputPipe[0]);
    close(_outputPipe[1]);

    std::vector<const char*> envp;
    for (size_t i = 0; i < _envVars.size(); ++i)
        envp.push_back(_envVars[i].c_str());
    envp.push_back(NULL);

    chdir(execDir.c_str());
    execle(execFile.c_str(), execFile.c_str(), NULL, &envp[0]);
    std::string error = "execle: " + std::string(strerror(errno)) + "\n";
    write(STDERR_FILENO, error.c_str(), error.length());
    _exit(1);
}

void CGIHandler::handleParentProcess(pid_t pid) {
    const Request& request = _response.getRequest();
    close(_inputPipe[0]);
    close(_outputPipe[1]);

    if (request.method() == Http::M_POST) {
        write(_inputPipe[1], request.body().data(), request.body().size());
    }
    close(_inputPipe[1]);

    std::string output = readCGIOutput(_outputPipe[0], pid);
    close(_outputPipe[0]);

    handleCGIOutput(output, pid);
}

std::string CGIHandler::readCGIOutput(int _outputPipeReadEnd, pid_t pid) {
    const int timeoutMillis = 5000;
    struct pollfd fds;
    fds.fd = _outputPipeReadEnd;
    fds.events = POLLIN;

    std::string output;
    char buffer[4096];

    while (true) {
        int pollResult = poll(&fds, 1, timeoutMillis);

        if (pollResult == 0) {
            kill(pid, SIGKILL);
            throw ExceptionMaker("CGI process timeout reached");
        } else if (pollResult < 0) {
            throw ExceptionMaker("Error polling CGI process output pipe");
        } else if (fds.revents & POLLIN) {
            ssize_t bytesRead = read(_outputPipeReadEnd, buffer, sizeof(buffer));
            if (bytesRead <= 0) break;
            output.append(buffer, bytesRead);
        } else {
            break;
        }
    }
    return output;
}

void CGIHandler::handleCGIOutput(const std::string& output, pid_t pid) {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        std::string body;
        std::map<std::string, std::string> headers;

        parseCGIOutput(output, headers, body);

        _response.setBody(body);
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            if (it->first == "Status") {
                _response.setStatusCode(static_cast<Http::STATUS_CODE>(Utils::stringToInt(it->second)));
            } else {
                _response.setHeader(it->first, it->second);
            }
        }
    } else {
        throw ExceptionMaker("CGI process exited with error status");
    }
}

void CGIHandler::parseCGIOutput(const std::string& output, std::map<std::string, std::string>& headers, std::string& body) {
    size_t headerEnd = output.find("\n\n");
    if (headerEnd == std::string::npos) {
        body = output;
        return;
    }

    std::string headerSection = output.substr(0, headerEnd);
    body = output.substr(headerEnd + 2);
    body.erase(0, body.find_first_not_of("\r\n"));

    std::istringstream headerStream(headerSection);
    std::string line;
    while (std::getline(headerStream, line)) {
        if (line.empty()) {
            continue;
        }
        size_t separatorPos = line.find(":");
        if (separatorPos != std::string::npos) {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            value.erase(0, value.find_first_not_of(" \t"));
            headers[key] = value;
        }
    }
}
