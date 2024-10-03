#pragma once

#include "../webserv.hpp"
#include "ServerLocation.hpp"


#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1024


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

		//	SETTERS FOR CONSTRUCTOR


		//			GETTERS

		uint32_t					getHost(void) const;
		uint16_t					getPort(void) const;
		std::string					getServerName(size_t i) const;
		ServerLocation				*getLocationFromIndex(size_t i) const;
		ServerLocation				*getLocationFromPath(std::string path) const;

		size_t						getServerNamesSize(void) const;
		size_t						getLocationBlocksSize(void) const;
		Utils::LOCATION_BLOCK_TYPE	getLocationType(ServerLocation *location) const;



	private:
		//			SETTERS
		uint32_t					_setHost(std::vector<std::string> strServerBlock);
		uint16_t					_setPort(std::vector<std::string> strServerBlock);
		void						_setServerName(std::vector<std::string> strServerBlock, std::vector<std::string> &serverNames);

		Utils::LOCATION_BLOCK_TYPE	parseLocationType(std::vector<std::string> strLocationBlock);


		uint32_t					_host;				//	listen IP[:PORT]
		uint16_t					_port;				//	listen [IP:]PORT
		std::vector<std::string>	_serverNames;		//	server_name SERVERNAME

		LocationBlocks				_locationBlocks;	//	location [PATH] {
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

