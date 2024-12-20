#pragma once

#include <string>
#include <vector>
#include <map>

#define DEFAULT_CONFIG_PATH	"configs/default.conf"
#define CHUNK_SIZE			4096
#define CRLF				"\r\n"

/******************************  CONTAINERS  ******************************/
typedef std::map<std::string, std::string>	StrStrMap;
typedef std::map<int, std::string>			IntStrMap;
typedef std::vector<std::string>			StrArr;
/**************************************************************************/

/********************************  CLASSES  *******************************/
class	ExceptionMaker;
class	ServerLocation;
class	SyntaxChecker;
class	ServerManager;
class	ConfigParser;
class	ServerConfig;
class	CGIHandler;
class	TCPSocket;
class	CGIMatch;
class	Response;
class	LogFeed;
class	Request;
/**************************************************************************/

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
