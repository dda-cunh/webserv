#pragma once
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <poll.h>
#include "../webserv.hpp"
#include "Request.hpp"
#include "Response.hpp"

class CGIHandler
{
	public:
		CGIHandler(Response &response, const std::string &cgiPath);
		~CGIHandler();

		void setEnvironmentVariables();
		void createPipes();
		void handleChildProcess();
		void handleParentProcess(pid_t pid);

	private:
		std::string readCGIOutput(int output_pipe, pid_t pid);
		void handleCGIOutput(const std::string &output, pid_t pid);
		void parseCGIOutput(const std::string &output, std::map<std::string, std::string> &headers, std::string &body);
		void closePipes();

		Response &					_response;
		const std::string & 		_cgiPath;
		int 						_inputPipe[2];
		int 						_outputPipe[2];
		std::vector<std::string>	_envVars;
};