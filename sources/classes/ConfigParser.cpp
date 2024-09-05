#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;
std::string					ConfigParser::_defaultRoot;
std::string					ConfigParser::_defaultIndex;


/*
static void	print_vector(std::vector<std::string> block)
{
	size_t	vectorSize;

	vectorSize = block.size();
	for (size_t i = 0; i < vectorSize; i++)
		std::cout << block.at(i) << std::endl;
	std::cout << "==================================" << std::endl;
}
*/

void	ConfigParser::parseConfigs(const char *path, ServerBlocks &configs)
{
	std::ifstream	configFile;
	std::string		line;

	configFile.open(path);
	if (!configFile.is_open())
		throw (ExceptionMaker("Unable to open configuration file") );

	//	READ configFile UNTIL EOF
	while (!configFile.eof() )
	{
		_loadServerContext(configFile);
		if (_strServerBlock.empty() )
			break ;


		//	FOR DEBUGGING
//		print_vector(_strServerBlock);
//		_strServerBlock.clear();


		//	ADD SYNTAX CHECK LATER


		_overrideDefaults();

		configs.push_back(serverConfig(_strServerBlock) );

		_strServerBlock.clear();
		_defaultRoot.clear();
		_defaultIndex.clear();
	}

	if (configs.empty() )
		throw (ExceptionMaker("Configuration file is empty") );
}

static void	erase_comments(std::string &line)
{
	size_t	pos;

	pos = line.find("#");
	if (pos != line.npos)
		line.erase(pos, line.size());
}

void	ConfigParser::_loadServerContext(std::ifstream &configFile)
{
	std::string	line;
	int			brackets;
	
	brackets = 0;

	//	SKIP FILE UNTIL START OF CONTEXT
	while (std::getline(configFile, line) )
	{
		erase_comments(line);
		Utils::sTrim(line);

		if (line.empty() )
			continue ;
		else if (line.find("server") == 0)
		{
			_strServerBlock.push_back(line);
			if (line.at(line.size() - 1) != '{')
			{
				std::getline(configFile, line);
				Utils::sTrim(line);

				if (line != "{")
					throw (ExceptionMaker("Syntax error in Server context encapsulation") );
				_strServerBlock.push_back(line);
			}
			brackets++;
			break ;
		}
		else
			throw (ExceptionMaker("Syntax error in Server context encapsulation") );
	}

	//	LOAD CONTEXT INTO STATIC VECTOR
	while (std::getline(configFile, line) )
	{
		erase_comments(line);
		Utils::sTrim(line);

		//	UPDATE BRACKET LEVEL
		if (line.empty() )
			continue ;
		else if (line.at(line.size() - 1) == '{')
			brackets++;
		else if (line.at(line.size() - 1) == '}')
			brackets--;

		if (!line.empty() )
			_strServerBlock.push_back(line);
		

		if (line.find("server") == 0)
			throw (ExceptionMaker("Syntax error in Server context encapsulation") );
		else if (brackets == 0)
			break ;
	}

	if (brackets != 0)
		throw (ExceptionMaker("Syntax error in Server context encapsulation") );
}


//		SYNTAX CHECK METHODS GO HERE


void	ConfigParser::_overrideDefaults(void)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = _strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = _strServerBlock.at(i);
		//	IF location CONTEXT IS FOUND, SKIP TO END OF location CONTEXT
		if (line.find("location") == 0)
		{
			while (line.at(line.size() - 1) != '}')
				i++;
		}
		//	SEARCH FOR root AND index DIRECTIVES
		//	THROW EXCEPTION IF MULTIPLE DEFAULTS ARE FOUND FOR SAME DIRECTIVE
		if (line.find("root") == 0)
		{
			if (_defaultRoot.empty() )
				//	COPY VALUE TO _defaultRoot
			else
				throw (ExceptionMaker("Multiple overrides for default root directive inside same server context") );
		}
		else if (line.find("index") == 0)
		{
			if (_defaultIndex.empty() )
				//	COPY VALUE TO _defaultIndex
			else
				throw (ExceptionMaker("Multiple overrides for default index directive inside same server context") );
		}
	}
}


/*		PARSING FOR SERVERCONFIG CLASS		*/
/*
uint32_t	ConfigParser::parseHost(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			//	GET 2ND WORD
			//	DETERMINE IF ':' IS PRESENT
			//	IF YES, GET VALUE AT LEFT SIDE
			//	ELSE, DETERMINE IF ITS AN IP ADDR
			//	RETURN IP IF YES, RETURN DEFAULT OTHERWISE
		}
	}

	return (Network::sToIPV4Packed(DEFAULT_HOST));
}

uint16_t	ConfigParser::parsePort(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			//	SAME AS PARSEHOST, BUT CHECK FOR PORT INSTEAD
		}
	}

	return (DEFAULT_PORT);
}

std::string	ConfigParser::parseServerName(std::vector<std::string> strServerBlock)
{
	size_t	vectorSize;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("server_name") == 0)
		{
			//	RETURN SECOND WORD
		}
	}

	return (DEFAULT_SERVER_NAME);
}
*/

/*		PARSING FOR SERVERLOCATION CLASSES		*/
/*
std::string	ConfigParser::parseLocation(std::vector<std::string> strLocationBlock)
{
	//	THIS CONFIG SHOULD BE ON THE 1ST LINE OF THE LOCATIONBLOCK
	//	CHECK IF THERE IS ANY WORD AFTER "LOCATION" (EXCLUDING '{' IF PRESENT)
	//	RETURN THAT VALUE IF FOUND

	return (DEFAULT_LOCATION);
}

std::string	ConfigParser::parseRootDir(std::vector<std::string> strLocationBlock)
{
	size_t	vectorSize;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strLocationBlock.at(i).find("root") == 0)
		{
			//	RETURN 2ND WORD FROM THIS LINE (EXCLUDING ';')
		}
	}

	if (this->_defaultRoot.empty() )
		return (DEFAULT_ROOT);
	else
		return (this->_defaultRoot);
}

std::string	ConfigParser::parseIndexFile(std::vector<std::string> strLocationBlock)
{
	size_t	vectorSize;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		//	RETURN 2ND WORD FROM THIS LINE (EXCLUDING ';')
	}

	if (this->_defaultIndex.empty() )
		return (DEFAULT_INDEX);
	else
		return (this->_defaultIndex);
}

*/