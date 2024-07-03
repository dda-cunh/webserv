#pragma once

#include "../webserv.hpp"

//	REMEMBER TO ADD SETTERS AND GETTERS (duh!)

class	ServerConfig
{
	public:
		ServerConfig();			// DONT FORGET TO SET DEFAULTS
		~ServerConfig(void);

	private:
		uint32_t		host;
		uint16_t		port;

		std::string		serverName;
		std::string 	rootDir;
		unsigned int	maxBodySize;
		ServerLocation	*location; // MAKE THIS A CONTAINER
};

class	ServerLocation
{
	public:
		ServerLocation(void);	// DONT FORGET TO SET DEFAULTS
		~ServerLocation(void);

	private:
		std::string	location;
		std::string	rootDir;
		std::string	indexDir;
		bool		autoIndex;
		//	ALLOWED METHODS AS FLAGS? OR MAYBE A <std::string, bool> MAP
		//	WILL NEED SOME KIND OF FLAGS TO DISTINGUISH BETWEEN STATIC SITE, CGI OR REVERSE PROXY
};