#pragma once

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <cstring>
#include <map>

#include "classes/ExceptionMaker.hpp"
#include "http.hpp"

namespace std
{ 
	typedef basic_string<unsigned char>			ustring;
}

typedef std::map<std::string, std::string>		StrStrMap;

typedef int										Socket;


#define FILE_BUFFER_SIZE						1024

/******************************  CLI_COLORS  ******************************/
#define CLI_COLORS_MAGENTA	"\033[35m"
#define CLI_COLORS_YELLOW	"\033[33m"
#define CLI_COLORS_BLACK	"\033[30m"
#define CLI_COLORS_GREEN	"\033[32m"
#define CLI_COLORS_WHITE	"\033[37m"
#define CLI_COLORS_RESET	"\033[0m"
#define CLI_COLORS_BLUE		"\033[34m"
#define CLI_COLORS_CYAN		"\033[36m"
#define CLI_COLORS_RED		"\033[31m"
/**************************************************************************/

/********************************  CLASSES  *******************************/
class	ExceptionMaker;

/*
	@ServerManager
	WAIT FOR ACCEPT
	PLACE CLIENT_FD ON EPOLL FOR READ
	GET REQUEST INSTANCE
	PLACE CLIENT_FD ON EPOLL FOR WRITE
	SEND RESPONSE
*/
class	ServerManager;
class	ServerConfig;
class	Response;
class	Request;
/**************************************************************************/

// TODO: Delete this
// TESTING WHILE WAITING FOR SERVER CONFIG IMPLEMENTATION
class ServerConfig
{
	public:
		Socket	socket;
};

/*********************************  UTILS  ********************************/
char	*seek_line(char *buffer, char *line, long bytes_read, int fd);
