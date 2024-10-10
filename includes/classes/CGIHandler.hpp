#pragma once
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include "../webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"

void setEnvironmentVariables(const std::string& cgiPath, const Request& request);
void createPipes(int input_pipe[2], int output_pipe[2]);
void handleChildProcess(int input_pipe[2], int output_pipe[2], const std::string& cgiPath);
void handleParentProcess(int input_pipe[2], int output_pipe[2], const Request& request, Response& response, pid_t pid);
