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


class	ConfigParser
{
	public:
		//	WRITE TEMP FUNCTION TO QUICKLY LOAD CONFIGS
		//		(PROPER PARSING TO BE IMPLEMENTED LATER)


			/*		PARSE FROM STR VECTOR TO SERVERCONFIG OBJECT		*/
		static uint32_t		parseHost(std::vector<std::string> strServerBlock);
		static uint16_t		parsePort(std::vector<std::string> strServerBlock);
		static std::string	parseServerName(std::vector<std::string> strServerBlock);

		//	ALSO GONNA NEED A FUNCTION TO DETERMINE LOCATION TYPE FROM VECTOR

			/*		PARSE FROM STR VECTOR TO SERVERLOCATION OBJECT		*/
		static std::string	parseLocation(std::vector<std::string> strLocationBlock);
		static std::string	parseRootDir(std::vector<std::string> strLocationBlock);
		static std::string	parseIndexFile(std::vector<std::string> strLocationBlock);
		static uint32_t		parseMaxBodySize(std::vector<std::string> strLocationBlock);
		static void			parseErrorPages(std::vector<std::string> strLocationBlock, IntStrMap &errorPages);
		static void			parseRedirections(std::vector<std::string> strLocationBlock, IntStrMap &redirections);
		static void			parseAllowedMethods(std::vector<std::string> strLocationBlock, std::vector<Http::METHOD> &methodsAllowed);

			/*		FOR LOCATIONSTATIC		*/
		static bool			parseAutoIndex(std::vector<std::string> strLocationBlock);


			/*		FOR LOCATIONCGI		*/




	private:
		ConfigParser(void);
		~ConfigParser(void);

		static std::vector<std::string>	_strServerBlock;
};