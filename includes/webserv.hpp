#pragma once

#include <sys/epoll.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <map>

#include "classes/ExceptionMaker.hpp"
#include "namespaces/Network.hpp"
#include "namespaces/Utils.hpp"
#include "namespaces/Http.hpp"

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
class	Socket;
/**************************************************************************/


typedef std::map<std::string, std::string>		StrStrMap;
typedef std::vector<unsigned char>				ByteArr;

#define CLIENT_CHUNK_SIZE						4096

//TODO: REMOVE PLACEHOLDER WHEN CLASS GETS IMPLEMENTED
struct ServerConfig
{
	uint32_t	ipv4;
	uint16_t	port;
};
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
