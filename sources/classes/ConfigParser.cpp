#include "../../includes/classes/ConfigParser.hpp"

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
			//	CLOSE FILE, THROW EXCEPTION
		//	CHECK IF SYNTAX IS OK FOR CURRENT SERVER CONTEXT LOADED INTO RAM
		if (!this->syntaxCheck(this->_strServerBlock))
			//	CLOSE FILE, THROW EXCEPTION
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
			if (configLine.empty())
				continue ;	//	empty lines are to be ignored
			else if (configLine.substr("server") == 0)
				//	load into RAM and return true if block syntax OK; false otherwise
			else
				return (false);	//	nonempty line with anything other than "server" is not valid config
		}
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