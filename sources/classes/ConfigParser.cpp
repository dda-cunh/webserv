#include "../../includes/classes/ConfigParser.hpp"

unsigned int	ConfigParser::_lineNr = 1;

void	ConfigParser::parseConfigs(std::string fileName, ServerBlocks &serverConfigs)
{
	std::ifstream	configFile;

	configFile.open(fileName.c_str());
	if (!configFile.is_open())
		//	THROW EXCEPTION

	while (!configFile.eof())
	{
		//	CHECK IF SERVER CONTEXT IS OK & LOAD IT TO MEMORY
		if (!this->serverContextOK(configFile))
			//	THROW EXCEPTION
		//	CHECK IF SYNTAX IS OK FOR CURRENT SERVER CONTEXT LOADED INTO RAM
		if (!this->syntaxCheck(this->_strServerBlock))
			//	THROW EXCEPTION
		//	LOAD FROM STRINGS TO STRUCT & PASS AS ARGUMENT TO CONFIG CLASS CONSTRUCTOR
		this->loadConfigs(this->_strServerBlock, serverConfigs);
	//	RESET this->_strServerBlock
	}
	configFile.close();

	if (!this->configsCheck(serverConfigs))
		//	THROW EXCEPTION

	//	LOAD DEFAULT CONFIGS IF FILE WAS EMPTY

}

bool	ConfigParser::serverContectOK(std::ifstream configFile)
{
	std::string	configLine;

	while (std::getline(configFile, configLine))
	{
		//	trim comments + blank spaces
		//	keep reading until a line with "server {" is found
		//	copy to this->_strServerBlock until closing bracket is found
		//	if no closing bracket found, clear vector & return false
	}

	return (true);
}

bool	ConfigParser::syntaxCheck(std::vector<std::string> strServerBlock)
{

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