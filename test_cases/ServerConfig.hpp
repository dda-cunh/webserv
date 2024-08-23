#pragma once

#include <vector>
#include <iostream>
#include "ServerLocation.hpp"

//	USE SOMETHING LIFO
typedef std::vector<ServerLocation *>	LocationBlocks;

// ADD ENUM FOT LOCATION TYPES

class	ServerConfig
{
	public:
		ServerConfig(void);
//		ServerConfig(std::vector<std::string> strServerBlock);
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);

		ServerConfig &operator = (const ServerConfig &serverConfig);

		//	SETTERS FOR CONSTRUCTOR


		//			GETTERS

		uint32_t			getHost(void);
		uint16_t			getPort(void);
		std::string			getServerName(void);
		size_t				getLocationBlocksSize(void);
		ServerLocation		*getLocationFromIndex(size_t i);
		ServerLocation		*getLocationFromPath(std::string path);
		LOCATION_BLOCK_TYPE getLocationType(ServerLocation *location);	//	MAYBE PUT THIS IN UTILS
		//	=============================

	private:
		uint32_t		_host;
		uint16_t		_port;	//	CHECK FOR OVERFLOWS ON INIT
		std::string		_serverName;

		LocationBlocks	_locationBlocks;
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

namespace Http
{
	typedef enum	e_method
	{
		M_UNHANDLED,
		M_DELETE,
		M_POST,
		M_GET,
		__LEN_METHOD
	}	METHOD;
}

typedef enum	e_location_block_type
{
	L_UNHANDLED,
	L_STATIC,
	L_REV_PROXY,
	L_CGI
}	LOCATION_BLOCK_TYPE;

