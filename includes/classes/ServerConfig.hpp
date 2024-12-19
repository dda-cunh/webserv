#pragma once

#include <stdint.h>

#include <vector>
#include <string>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1024


class ServerLocation;

typedef std::vector<ServerLocation *>			LocationBlocks;


class	ServerConfig
{
	public:
		ServerConfig(void);
		ServerConfig(std::vector<std::string> strServerBlock);
		ServerConfig(const ServerConfig &serverConfig);
		~ServerConfig(void);

		ServerConfig &operator = (const ServerConfig &serverConfig);

		uint32_t					getHost(void) const;
		uint16_t					getPort(void) const;
		std::string					getServerName(size_t i) const;
		ServerLocation				*getLocationFromIndex(size_t i) const;
		ServerLocation				*getLocationFromPath(std::string path) const;

		size_t						getServerNamesSize(void) const;
		size_t						getLocationBlocksSize(void) const;


	private:
		//			SETTERS
		uint32_t					_setHost(std::vector<std::string> strServerBlock);
		uint16_t					_setPort(std::vector<std::string> strServerBlock);
		void						_setServerName(std::vector<std::string> strServerBlock, std::vector<std::string> &serverNames);

		uint32_t					_host;				//	listen IP[:PORT]
		uint16_t					_port;				//	listen [IP:]PORT
		std::vector<std::string>	_serverNames;		//	server_name SERVERNAME

		LocationBlocks				_locationBlocks;	//	location [PATH] {
};

std::ostream & operator << (std::ostream &out, const ServerConfig &serverConfig);

std::ostream & operator << (std::ostream &out, const ServerLocation &location);