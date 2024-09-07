#pragma once

#include "../webserv.hpp"
#include "ConfigParser.hpp"
#include "ServerLocation.hpp"

class ServerLocation;
class LocationStatic;

typedef std::vector<ServerLocation *>			LocationBlocks;

typedef enum	e_location_block_type
{
	L_UNHANDLED,
	L_STATIC,
	L_REV_PROXY,
	L_CGI
}	LOCATION_BLOCK_TYPE;

class	ServerConfig
{
	public:
		ServerConfig(void);
		ServerConfig(std::vector<std::string> strServerBlock);
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);

		ServerConfig &operator = (const ServerConfig &serverConfig);

		//	SETTERS FOR CONSTRUCTOR


		//			GETTERS

		uint32_t			getHost(void) const;
		uint16_t			getPort(void) const;
		std::string			getServerName(void) const;
		size_t				getLocationBlocksSize(void) const;
		ServerLocation		*getLocationFromIndex(size_t i) const;
		ServerLocation		*getLocationFromPath(std::string path) const;
		LOCATION_BLOCK_TYPE getLocationType(ServerLocation *location) const;	//	MAYBE PUT THIS IN UTILS
		//	=============================

	private:
		uint32_t		_host;			//	listen
		uint16_t		_port;			//	port
		std::string		_serverName;	//	server_name

		LocationBlocks	_locationBlocks;
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

