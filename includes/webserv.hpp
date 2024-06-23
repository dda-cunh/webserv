#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <iostream>
#include <map>

#include "classes/ExceptionMaker.hpp"
#include "http.hpp"

namespace std
{ 
	typedef basic_string<unsigned char>			ustring;
}

typedef std::map<std::ustring, std::ustring>	UStrUStrMap;

typedef int										Socket;

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
class	ServerConfig;
class	Response;
class	Request;
class	Server;
/**************************************************************************/

// TESTING WHILE WAITING FOR SERVER CONFIG IMPLEMENTATION
class ServerConfig
{
	public:
		Socket	socket;
};
//----------------------------------------------------
#endif
