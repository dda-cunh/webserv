#include "../../includes/classes/ConfigParser.hpp"

unsigned int	ConfigParser::_lineNr = 1;

void	ConfigParser::parseConfigs(std::string fileName, ServerBlocks &serverConfigs)
{
	std::ifstream	configFile;

	configFile.open(fileName.c_str());
	if (!configFile.is_open())
		//	THROW EXCEPTION

//	LINENR MUST BE UPDATED IN CASE OF EXCEPTION
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

bool	ConfigParser::serverContextOK(std::ifstream configFile)
{
	std::string	configLine;

	while (std::getline(configFile, configLine))
	{
		if (!configLine.empty())
		{
			//	trim comments + blank spaces
			trimConfigLine(configLine);
			//	keep reading until a line with "server {" is found and
			//		copy to this->_strServerBlock until closing bracket is found
		}
	}
	//	if no closing bracket was found, clear vector & return false

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

void	trimConfigLine(std::string &configLine) // not util, put this in class scope
{
	size_t	pos;

	if (configLine.empty())
		return ;

	//	REMOVE COMMENTS
	pos = configLine.find("#");
	if (pos != configLine.npos)
		configLine.erase(pos, configLine.size());
	if (configLine.empty())
		return ;

	//	REMOVE ' ' AND '\t' FROM LEFT
	pos = configLine.find_first_not_of(" \t");
	if (pos != configLine.npos)
		configLine.erase(0, pos);
	if (configLine.empty())
		return ;

	//	REMOVE ' ' AND '\t' FROM RIGHT
	pos = configLine.find_last_not_of(" \t");
	if (pos != configLine.npos)
		configLine.erase(pos + 1, configLine.size());
}