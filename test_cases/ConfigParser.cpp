#include "ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;

static void	printStrConfig(std::vector<std::string> &strServerConfig)
{
	for (std::vector<std::string>::iterator bgn = strServerConfig.begin(); bgn < strServerConfig.end(); bgn++)
		std::cout << *bgn << std::endl;
}

void	ConfigParser::parseConfigs(std::string fileName /*, ServerBlocks &serverConfigs*/)	//	UNCOMMENT
{
	std::ifstream	configFile;

	configFile.open(fileName.c_str());
	if (!configFile.is_open())
	{
		//	THROW EXCEPTION
		//	TEST CODE
		return ;
		//	=========
	}

	while (!configFile.eof())
	{
		//	CHECK IF SERVER CONTEXT IS OK & LOAD IT TO MEMORY
		if (!serverBlockOK(configFile))
		{
			configFile.close();
			//	THROW EXCEPTION
			//	TEST CODE
			std::cout << "SERVER BLOCK NOT OK" << std::endl;
			return ;
			//	=========
		}
		//	LOAD FROM STRINGS TO STRUCT & PASS AS ARGUMENT TO CONFIG CLASS CONSTRUCTOR
//		loadConfigs(_strServerBlock, serverConfigs);	UNCOMMENT
		printStrConfig(_strServerBlock);
		_strServerBlock.clear();
	}
	configFile.close();

//	if (!configsCheck(serverConfigs))	UNCOMMENT
		//	THROW EXCEPTION



	//	LOAD DEFAULT CONFIGS IF FILE WAS EMPTY

}

	/*	Utils for ParseConfigs	*/

bool	ConfigParser::serverBlockOK(std::ifstream &configFile)
{
	std::string	configLine;

	while (std::getline(configFile, configLine))
	{
		//	trim comments + blank spaces
		trimConfigLine(configLine);
		if (configLine.empty())
			continue ;	//	empty lines are to be ignored
		else if (serverBlockHeaderOK(configLine))
		{
			//	load into RAM and check if block syntax OK
			_strServerBlock.push_back(configLine);
			if (!copyToVector(configFile/*, _strServerBlock*/) /* || !syntaxCheck(_strServerBlock) */)	//	UNCOMMENT
				return (false);
		}
		else if (!configFile.eof() && !configLine.empty())
			return (false);	//	nonempty line with anything other than "server {" is not valid config
	}
	return (true);
}
	/*	==============================	*/

	/*	UTILS FOR serverBlockOK()	*/
void	ConfigParser::trimConfigLine(std::string &configLine) // not util, put this in class scope
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
		configLine.erase(pos + 1, configLine.size());	//	WILL IT SEGFAULT?
}

bool	ConfigParser::serverBlockHeaderOK(std::string configLine)
{
	std::istringstream	wordStream(configLine);
	std::string			word1;
	std::string			word2;

	wordStream >> word1;
	wordStream >> word2;
	if (wordCount(configLine) == 2 \
		&& word1 == "server" && word2 == "{")
		return (true);	
	else
		return (false);
}

bool	ConfigParser::copyToVector(std::ifstream &configFile/*, std::vector<std::string> strBlock*/)	//	UNCOMMENT
{
	std::string		configLine;
	int				braceLvl;

	braceLvl = 1;
	while (std::getline(configFile, configLine))
	{
		trimConfigLine(configLine);
		if (!configLine.empty())
		{
			switch (configLine.at(configLine.size() - 1))
			{
				case ('{'):
					braceLvl++;
					_strServerBlock.push_back(configLine);
					break ;
				case ('}'):
					braceLvl--;
					_strServerBlock.push_back(configLine);
					break ;
				case (';'):
					_strServerBlock.push_back(configLine);
					break ;
				default:
					if (configLine.find("location ") != 0)
						return (false);										
					else
						_strServerBlock.push_back(configLine);
			}
			if (braceLvl == 0)	
				return (true);
		}
	}
	return (false);	//	reached EOF and braceLvl != 0
}
/*
bool	ConfigParser::syntaxCheck(std::vector<std::string> strServerBlock)
{

	return (true);
}
*/	//	UNCOMMENT
/*	==============================	*/

	/*	UTILS FOR serverBlockHeaderOK()	*/
unsigned int		ConfigParser::wordCount(std::string configLine)
{
	std::istringstream	strStream(configLine);
	std::string			word;
	unsigned int		wordsCounted;

	wordsCounted = 0;
	while (strStream >> word)
		wordsCounted++;
	return (wordsCounted);
}
/*
void	ConfigParser::loadConfigs(std::string fileName, / CONFIGS CONTAINER *)
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

bool	ConfigParser::configsCheck(/ CONFIGS CONTAINER /)
{
	//	ARE WE REALLY GONNA NEED THIS?
}
*/
ConfigParser::ConfigParser(void)
{
	return ;
}

ConfigParser::~ConfigParser(void)
{
	return ;
}

