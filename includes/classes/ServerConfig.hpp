#pragma once

#include <fstream>	//	what do I need this for again?
#include <vector>
#include "ServerLocation.hpp"
#include "../webserv.hpp"

//	USE SOMETHING LIFO
typedef std::vector<ServerLocation *>	LocationBlocks;

// ADD ENUM FOT LOCATION TYPES

class	ServerConfig
{
	public:
		ServerConfig(void);			//	DONT FORGET TO SET DEFAULTS
		ServerConfig(std::vector<std::string> strServerBlock);
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);	//	DESTRUCTOR MUST DELETE POINTERS IN locationBlocks

		ServerConfig &operator = (const ServerConfig &serverConfig);

		//	SETTERS FOR CONSTRUCTOR

		/*
			ADD THESE METHODS:
				getLocationFromPath(std::string path) - returns const ref to locationBlock corresponting to path; if not found: return NULL or exception?
					CHECK WITH NGINX HOW PATH TREES ARE HANDLED & HOW TO LIMIT ACCESS TO PATHS
				getLocationType(ServerLocation *location) - return corresponding type from enum
					IMPLEMENT ENUM FOR DIFFERENT TYPES OF SERVER LOCATION
		*/

		//			GETTERS
		std::string		getServerName(void);
		std::string		getRootDir(void);
		unsigned int	getMaxBodySize(void);
		ServerLocation	*getLocationFromPath(std::string path);
		int 			getLocationType(ServerLocation *location);	//	MAYBE PUT THIS IN UTILS
		//	=============================

	private:
		uint32_t		_host;
		uint16_t		_port;	//	CHECK FOR OVERFLOWS ON INIT

		std::string		_serverName;
		unsigned int	_maxBodySize;

		LocationBlocks	_locationBlocks;
	
		//	THIS ONE, ALONGSIDE WITH OTHER KEYWORDS THAT CAN GO ON LOCATION
		//	BLOCKS, WILL SERVE AS "OVERRIDE" FOR LOCATION DEFAULTS
		//	THEREFORE, THIS MEMBER WILL BE REMOVED
		//	(AND I'LL PROB NEED A STRUCT TO HANDLE DEFAULT OVERRIDES OR STH)
		std::string 	_rootDir;
};

typedef enum	e_location_type
{
	L_UNHANDLED,
	L_STATIC,
	L_CGI
}	LOCATION_TYPE;