#include "../../includes/classes/CGIHandler.hpp"

void setEnvironmentVariables(const std::string& cgiPath, const Request& request, Response& response, std::vector<std::string>& envVars) {
    std::string pathInfo = request.uri().substr(request.uri().find(".cgi") + 4);
    
    envVars.push_back("REQUEST_METHOD=" + Http::methodToString(request.method()));
    envVars.push_back("PATH_INFO=" + pathInfo);
    envVars.push_back("SCRIPT_FILENAME=" + cgiPath);
    envVars.push_back("UPLOAD_DIR=" + response.getMatchedLocation()->getUploadPath());

    if (request.method() == Http::M_POST) {
        envVars.push_back("CONTENT_TYPE=" + request.header("Content-Type"));
        envVars.push_back("CONTENT_LENGTH=" + request.header("Content-Length"));
    }
}
void createPipes(int input_pipe[2], int output_pipe[2]) {
    if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1)
        throw ExceptionMaker("Pipe creation failed: " + std::string(strerror(errno)));
}

void handleChildProcess(int input_pipe[2], int output_pipe[2], const std::string& cgiPath, const std::vector<std::string>& envVars) {
    close(input_pipe[1]);
    close(output_pipe[0]);

    std::string execDir = cgiPath.substr(0, cgiPath.find_last_of('/') + 1);
    std::string execFile = cgiPath.substr(cgiPath.find_last_of('/') + 1);

    dup2(input_pipe[0], STDIN_FILENO);
    dup2(output_pipe[1], STDOUT_FILENO);
    dup2(output_pipe[1], STDERR_FILENO);

    close(input_pipe[0]);
    close(output_pipe[1]);

    std::vector<const char*> envp;
    for (size_t i = 0; i < envVars.size(); ++i)
        envp.push_back(envVars[i].c_str());
    envp.push_back(NULL);

    chdir(execDir.c_str());
    execle(execFile.c_str(), execFile.c_str(), NULL, &envp[0]);
    throw ExceptionMaker("execle failed: " + std::string(strerror(errno)));
}

void handleParentProcess(int input_pipe[2], int output_pipe[2], const Request& request, Response& response, pid_t pid) {
    close(input_pipe[0]);
    close(output_pipe[1]);

    if (request.method() == Http::M_POST) {
        write(input_pipe[1], request.body().data(), request.body().size());
    }
    close(input_pipe[1]);

    char buffer[4096];
    ssize_t bytesRead;
    std::string output;

    while ((bytesRead = read(output_pipe[0], buffer, sizeof(buffer))) > 0) {
        output.append(buffer, bytesRead);
    }
    close(output_pipe[0]);

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        std::string body;
        std::map<std::string, std::string> headers;

        parseCGIOutput(output, headers, body);

        response.setBody(body);
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
            response.setHeader(it->first, it->second);
        }
    } else {
        throw ExceptionMaker("CGI script exited with error status: " + Utils::intToString(WEXITSTATUS(status)));
    }
}

void parseCGIOutput(const std::string& output, std::map<std::string, std::string>& headers, std::string& body) {
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
