#pragma once

#include "ServerManager.hpp"
#include "SyntaxChecker.hpp"


typedef std::vector<ServerConfig>	ServerBlocks;


class	ConfigParser
{
	public:
		static void			parseConfigs(const char *path, ServerBlocks &configs);


			/*		PARSE FROM STR VECTOR TO SERVERCONFIG OBJECT		*/
		static uint32_t				parseHost(std::vector<std::string> strServerBlock);
		static uint16_t				parsePort(std::vector<std::string> strServerBlock);
		static std::string			parseServerName(std::vector<std::string> strServerBlock);

		static Utils::LOCATION_BLOCK_TYPE	parseStrLocationType(std::vector<std::string> strLocationBlock);


			/*		PARSE FROM STR VECTOR TO SERVERLOCATION OBJECT		*/
		static std::string	parseLocation(std::string locationLine);
		static std::string	parseRootDir(std::vector<std::string> strLocationBlock);
		static void			parseIndexFiles(std::vector<std::string> strLocationBlock, std::vector<std::string> &indexFiles);
		static uint32_t		parseMaxBodySize(std::vector<std::string> strLocationBlock);
		static void			parseErrorPages(std::vector<std::string> strLocationBlock, IntStrMap &errorPages);
		static void			parseRedirections(std::vector<std::string> strLocationBlock, StrStrMap &redirections);
		static void			parseAllowedMethods(std::vector<std::string> strLocationBlock, std::vector<Http::METHOD> &methodsAllowed);

			/*		FOR LOCATIONSTATIC		*/
		static bool			parseAutoIndex(std::vector<std::string> strLocationBlock);


			/*		FOR LOCATIONCGI		*/




	private:
		ConfigParser(void);
		~ConfigParser(void);

		static void	_loadServerContext(std::ifstream &configFile);
//		static void	_syntaxCheck WILL BE ADDED LATER
		static void	_overrideDefaults(void);


		static std::vector<std::string>	_strServerBlock;

		static std::string				_defaultRoot;
		static std::vector<std::string>	_defaultIndex;
		static uint32_t					_defaultMaxBodySize;
		static IntStrMap				_defaultErrorPages;
		static StrStrMap				_defaultRedirections;
		static std::vector<std::string>	_defaultMethodsAllowed;
};