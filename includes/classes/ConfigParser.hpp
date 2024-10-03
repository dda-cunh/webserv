#pragma once

#include "ServerManager.hpp"
#include "SyntaxChecker.hpp"


typedef std::vector<ServerConfig>	ServerBlocks;


class	ConfigParser
{
	public:
		static void		parseConfigs(const char *path, ServerBlocks &configs);

		static void		strToVecSplit(std::string line, std::vector<std::string> &strVector);


		static std::string				defaultRoot;
		static std::vector<std::string>	defaultIndex;
		static uint32_t					defaultMaxBodySize;
		static IntStrMap				defaultErrorPages;
		static StrStrMap				defaultRedirections;
		static std::vector<std::string>	defaultMethodsAllowed;
		static std::string				defaultUploadPath;

		static bool						defaultAutoIndex;
//		static StrStrMap				defaultCgiPass;


	private:
		ConfigParser(void);
		~ConfigParser(void);

		static void							_loadServerContext(std::ifstream &configFile);
		static void							_overrideDefaults(void);

		static std::vector<std::string>		_strServerBlock;

};