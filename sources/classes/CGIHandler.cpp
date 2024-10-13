#include "../../includes/classes/CGIHandler.hpp"

void setEnvironmentVariables(const std::string& cgiPath, const Request& request, std::vector<std::string>& envVars) {
    std::string pathInfo = request.uri().substr(request.uri().find(".cgi") + 4);
    
    envVars.push_back("REQUEST_METHOD=" + Http::methodToString(request.method()));
    envVars.push_back("PATH_INFO=" + pathInfo);
    envVars.push_back("SCRIPT_FILENAME=" + cgiPath);

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

    dup2(input_pipe[0], STDIN_FILENO);
    dup2(output_pipe[1], STDOUT_FILENO);
    dup2(output_pipe[1], STDERR_FILENO);

    close(input_pipe[0]);
    close(output_pipe[1]);

    std::vector<const char*> envp;
    for (size_t i = 0; i < envVars.size(); ++i) {
        envp.push_back(envVars[i].c_str());
    }
    envp.push_back(NULL);
    execle("/usr/bin/python3", "python3", cgiPath.c_str(), NULL, &envp[0]);
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
        std::string contentType;
        std::string body;

        parseCGIOutput(output, contentType, body);

        response.setBody(body);
        response.setHeader("Content-Type", contentType);
    } else {
        throw ExceptionMaker("CGI script exited with error status: " + Utils::intToString(WEXITSTATUS(status)));
    }
}

void parseCGIOutput(const std::string& output, std::string& contentType, std::string& body) {
    size_t headerEnd = output.find("\n");
    if (headerEnd == std::string::npos) {
        body = output;
        contentType = "text/plain";
        return;
    }

    std::string header = output.substr(0, headerEnd);
    body = output.substr(headerEnd + 1);
    body.erase(0, body.find_first_not_of("\r\n"));

    size_t contentTypePos = header.find("Content-Type:");
    if (contentTypePos != std::string::npos) {
        contentType = header.substr(contentTypePos + 13);
        contentType.erase(0, contentType.find_first_not_of(" \t"));
        contentType.erase(contentType.find_last_not_of(" \t") + 1);
    } else {
        contentType = "text/plain";
    }
}