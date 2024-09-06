#pragma once

#include "../webserv.hpp"

/*	DEFAULTS FOR SERVERCONFIG	*/
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 1024
#define DEFAULT_SERVER_NAME "webserv.ft"

/*	DEFAULTS FOR SERVERLOCATION	*/
#define DEFAULT_LOCATION "/"
#define DEFAULT_ROOT "./test_files"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_MAX_BODY_SIZE 1000000

#define DEFAULT_AUTO_INDEX false

class	ServerConfig;
typedef std::vector<ServerConfig>	ServerBlocks;


class	ConfigParser
{
	public:
		static void			parseConfigs(const char *path, ServerBlocks &configs);


	private:
		ConfigParser(void);
		~ConfigParser(void);

		static void	_loadServerContext(std::ifstream &configFile);
		static void	_overrideDefaults(void);


		static std::vector<std::string>	_strServerBlock;

		static std::string	_defaultRoot;
		static std::string	_defaultIndex;
};