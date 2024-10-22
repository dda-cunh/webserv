#pragma once
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include "../webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"

void setEnvironmentVariables(const std::string& cgiPath, Response& response, std::vector<std::string>& envVars);
void createPipes(int inputPipe[2], int outputPipe[2]);
void handleChildProcess(int inputPipe[2], int outputPipe[2], Response& response, const std::vector<std::string>& envVars);
void handleParentProcess(int inputPipe[2], int outputPipe[2], const Request& request, Response& response, pid_t pid);
void parseCGIOutput(const std::string& output, std::map<std::string, std::string>& headers, std::string& body);