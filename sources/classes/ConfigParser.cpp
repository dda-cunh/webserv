#include "../../includes/classes/ConfigParser.hpp"

unsigned int	ConfigParser::_lineNr = 1;

void	ConfigParser::parseConfigs(std::string fileName)
{
	std::ifstream	configFile;
	configFile.open(fileName.c_str());
	if (!configFile.is_open())
		//	THROW EXCEPTION

	//	LOAD FILE INTO STRUCT
	if (!this->serverContectOK(configFile))
		//	THROW EXCEPTION
	if (!this->syntaxCheck(this->_strServerBlock))
		//	THROW EXCEPTION
	this->loadConfigs(fileName, this->_strServerBlock);
	//	REPEAT UNTIL EOF

	if (!this->configsCheck(/* CONFIGS CONTAINER */))
		//	THROW EXCEPTION
}

bool	ConfigParser::syntaxCheck(std::string fileName)
{
	std::ifstream	configFile;
	std::string		configLine;

	while (std::getline(configFile, configLine))
	{
		//	check for valid keywords, syntax, context, configs, etc
		//	if an invalid line is found, return false
		this->_lineNr++;
	}
	return (true);
}

void	ConfigParser::loadConfigs(std::string fileName, /* CONFIGS CONTAINER */)
{
	std::ifstream	configFile;
	std::string		configLine;

	configFile.open(fileName.c_str());
	if (!configFile.is_open())
		//	throw exception
	while (std::getline(configFile, configLine))
	{
		//	check keyword, load into memory
	}
}

bool	ConfigParser::configsCheck(/* CONFIGS CONTAINER */)
{
	//	ARE WE REALLY GONNA NEED THIS?
}

ConfigParser::ConfigParser(void)
{
	return ;
}

ConfigParser::~ConfigParser(void)
{
	return ;
}

//	UTIL FUNCTIONS; MOVE TO UTILS.HPP BEFORE MERGE
int	wordCount(std::string configLine)
{
	std::istringstream	strStream(configLine);
	std::string			word;
	int					wordsCounted;

	words = 0;
	while (strStream >> word)
		wordsCounted++;
	return (wordsCounted);
}