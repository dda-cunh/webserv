#pragma once

#include "../webserv.hpp"
#include "ServerLocation.hpp"


#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1024
#define DEFAULT_SERVER_NAME "webserv.ft"


class ServerLocation;
class LocationStatic;

typedef std::vector<ServerLocation *>			LocationBlocks;


class	ServerConfig
{
	public:
		ServerConfig(void);
		ServerConfig(std::vector<std::string> strServerBlock);
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);

		ServerConfig &operator = (const ServerConfig &serverConfig);

		uint32_t			getHost(void) const;
		uint16_t			getPort(void) const;
		std::string			getServerName(void) const;
		size_t				getLocationBlocksSize(void) const;
		ServerLocation		*getLocationFromIndex(size_t i) const;
		ServerLocation		*getLocationFromPath(std::string path) const;
		Utils::LOCATION_BLOCK_TYPE getLocationType(ServerLocation *location) const;

	private:								//	KEYWORDS
		uint32_t		_host;				//	listen
		uint16_t		_port;				//	listen
		std::string		_serverName;		//	server_name

		LocationBlocks	_locationBlocks;	//	location
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

