#pragma once

#include "../webserv.hpp"

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
		CGIHandler(const CGIHandler&);
		CGIHandler& operator=(const CGIHandler&);

	    enum {
			BUFFER_SIZE = 4096,
			CGI_TIMEOUT_MS = 3000
    	};
		
		std::string readCGIOutput(int output_pipe, pid_t pid);
		void handleCGIOutput(const std::string &output, pid_t pid);
		void parseCGIOutput(const std::string &output, std::map<std::string, std::string> &headers, std::string &body);

		Response &					_response;
		const std::string & 		_cgiPath;
		int 						_inputPipe[2];
		int 						_outputPipe[2];
		std::vector<std::string>	_envVars;
};