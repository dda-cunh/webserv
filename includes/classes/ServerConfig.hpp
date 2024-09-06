#pragma once

#include "../webserv.hpp"
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
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);

		ServerConfig &operator = (const ServerConfig &serverConfig);

		uint32_t			getHost(void) const;
		uint16_t			getPort(void) const;
		std::string			getServerName(void) const;
		size_t				getLocationBlocksSize(void) const;
		ServerLocation		*getLocationFromIndex(size_t i) const;
		ServerLocation		*getLocationFromPath(std::string path) const;
		LOCATION_BLOCK_TYPE getLocationType(ServerLocation *location) const;

	private:
		uint32_t		_host;
		uint16_t		_port;
		std::string		_serverName;

		LocationBlocks	_locationBlocks;
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

