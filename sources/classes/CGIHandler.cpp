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
    _envVars.push_back("QUERY_STRING=" + _response.getCGIMatch().getQueryString());

    if (request.method() == Http::M_POST) {
        _envVars.push_back("CONTENT_TYPE=" + request.header("Content-Type"));
        _envVars.push_back("CONTENT_LENGTH=" + request.header("Content-Length"));
    }

    LOG("CGI process env variables:", Utils::LOG_INFO);
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

    std::string execDir = Utils::concatenatePaths(_response.getLocationMatch()->getRootDir().c_str(),
                                                    _response.getCGIMatch().getBasePath().c_str(),
                                                    NULL);
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

    if (request.method() == Http::M_POST)
        if (write(_inputPipe[1], request.body().data(), request.body().size()) == -1)
            throw ExceptionMaker("Write to CGI input pipe failed: " + std::string(strerror(errno)));

    std::string output = readCGIOutput(_outputPipe[0], pid);
    handleCGIOutput(output, pid);
}

std::string CGIHandler::readCGIOutput(int output_pipe, pid_t pid) {
    struct pollfd fds;
    fds.fd = output_pipe;
    fds.events = POLLIN;
    fds.revents = 0;

    std::string output;
    char buffer[BUFFER_SIZE];

    while (true) {
        int pollResult = poll(&fds, 1, CGI_TIMEOUT_MS);

        if (pollResult == 0) {
            kill(pid, SIGKILL);
            throw ExceptionMaker("CGI process timeout reached");
        }
        if (pollResult < 0) {
            if (errno == EINTR) continue;
            throw ExceptionMaker("Poll failed: " + std::string(strerror(errno)));
        }
        if (!(fds.revents & POLLIN)) break;

        ssize_t bytesRead = read(output_pipe, buffer, BUFFER_SIZE);
        if (bytesRead <= 0) break;
        output.append(buffer, bytesRead);
    }

    return output;
}

void CGIHandler::handleCGIOutput(const std::string& output, pid_t pid) {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0)
            LOG("CGI process exited with error status " + Utils::intToString(WEXITSTATUS(status)), Utils::LOG_WARNING);
        
        std::string body;
        std::map<std::string, std::string> headers;

        parseCGIOutput(output, headers, body);

        _response.setBody(body);
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            if (it->first == "Status")
                _response.setStatusCode(static_cast<Http::STATUS_CODE>(Utils::stringToInt(it->second)));
            else
                _response.setHeader(it->first, it->second);
        }
    } else {
        throw ExceptionMaker("CGI process exited with error status" + Utils::intToString(WEXITSTATUS(status)));
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
