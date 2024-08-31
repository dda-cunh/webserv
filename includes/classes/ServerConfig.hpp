#pragma once

#include "../webserv.hpp"

class ServerLocation;
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
//		ServerConfig(std::vector<std::string> strServerBlock);
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
		uint32_t		_host;
		uint16_t		_port;	//	CHECK FOR OVERFLOWS ON INIT
		std::string		_serverName;

		LocationBlocks	_locationBlocks;
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

