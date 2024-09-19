#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;

std::string					ConfigParser::_defaultRoot;
std::vector<std::string>	ConfigParser::_defaultIndex;
IntStrMap					ConfigParser::_defaultErrorPages;


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

static void	erase_comments(std::string &line)
{
	size_t	pos;

	pos = line.find("#");
	if (pos != line.npos)
		line.erase(pos, line.size());
}

static std::string	str_parse_line(std::string line)
{
	std::string	val;

	val = line;

	val.erase(0, val.find_first_of(" \t") );
	if (val.find_last_of("{;") != val.npos)
		val.erase(val.find_last_of("{;"), 1);
	val = Utils::sTrim(val);

	return (val);
}

static size_t	word_count(std::string line)
{
	std::istringstream	strStream(line);
	std::string			outStr;
	size_t				count;

	count = 0;
	while (strStream >> outStr)
		count++;

	return (count);
}

static void	split_string_to_vector(std::string line, std::vector<std::string> &strVector)
{
	std::istringstream	strStream(line);
	std::string			outStr;

	while(strStream >> outStr)
		strVector.push_back(outStr);
}

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



		//	ADD SYNTAX CHECK LATER



		_overrideDefaults();
		
		//	FOR DEBUGGING
/*		print_vector(_strServerBlock);
		std::cout << "Default override for root: " << _defaultRoot << std::endl;
		std::cout << "Default override for index: " << _defaultIndex << std::endl;
*/
		configs.push_back(ServerConfig(_strServerBlock) );
	

		_strServerBlock.clear();
		_defaultRoot.clear();
		_defaultIndex.clear();
		_defaultErrorPages.clear();
	}

	configFile.close();
	if (configs.empty() )
		throw (ExceptionMaker("Configuration file is empty") );

	//	DO ONE FINAL PASS ON configs TO CHECK FOR INVALID CONFIGS
	//	(SUCH AS DUPLICATE LOCATION BLOCKS)
/*
	for (size_t i = 0; i < configs.size(); i++)
		std::cout << configs.at(i) << std::endl;
*/

	for (size_t i = 0; i < configs.size(); i++)
	{
		std::cout << configs.at(i) << std::endl;
		std::cout << "=============================" << std::endl;
	}
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
				line = Utils::sTrim(line);

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
		line = Utils::sTrim(line);

		//	UPDATE BRACKET LEVEL
		if (line.empty() )
			continue ;
		else if (line.at(line.size() - 1) == '{')
			brackets++;
		else if (line.at(line.size() - 1) == '}')
			brackets--;

		if (!line.empty() )
			_strServerBlock.push_back(line);
		

		if (line.find("server ") == 0)
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
	int			nVal;
	std::string	line;

	vectorSize = _strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = _strServerBlock.at(i);
		//	IF location CONTEXT IS FOUND, SKIP TO END OF location CONTEXT
		if (line.find("location") == 0)
		{
			while (line.at(line.size() - 1) != '}')
			{
				i++;
				line = _strServerBlock.at(i);
			}
		}
		//	SEARCH FOR root AND index DIRECTIVES
		//	THROW EXCEPTION IF MULTIPLE DEFAULTS ARE FOUND FOR root DIRECTIVE
		if (line.find("root") == 0)
		{
			if (_defaultRoot.empty() )
			{
				line = str_parse_line(line);
				if (word_count(line) != 1)
					throw (ExceptionMaker("Invalid number of arguments in \"root\" directive") );
				_defaultRoot = line;
			}
			else
				throw (ExceptionMaker("Multiple overrides for default root directive inside same server context") );
		}
		else if (line.find("index") == 0)
		{
			line = str_parse_line(line);
			split_string_to_vector(line, _defaultIndex);
		}
		else if (line.find("error_page") == 0)
		{
			line = str_parse_line(line);
			if (word_count(line) != 2)
				throw (ExceptionMaker("Invalid number of arguments in \"error_page\" directive") );
			nVal = std::atoi(line.substr(0, line.find_first_of(" \t") ).c_str() );
			if (_defaultErrorPages.find(nVal) == _defaultErrorPages.end() )
				_defaultErrorPages[nVal] = line.substr(line.find_last_of(" \t") + 1);
			else
				throw (ExceptionMaker("Multiple settings for the same error page in server context") );
		}
	}
}


/*		PARSING FOR SERVERCONFIG CLASS		*/

uint32_t	ConfigParser::parseHost(std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	strHost;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			strHost = str_parse_line(strServerBlock.at(i) );
			while (++i < vectorSize)
			{
				if (strServerBlock.at(i).find("listen") == 0)
					throw (ExceptionMaker("\"listen\" directive is duplicate") );
			}

			//	GET 2ND WORD
			//	DETERMINE IF ':' IS PRESENT
			//		IF YES, GET VALUE AT LEFT SIDE
			if (strHost.find(':') != strHost.npos)
				return (Network::sToIPV4Packed(strHost.substr(0, strHost.find(':') ) ) );
			else
			{
			//		ELSE, DETERMINE IF ITS AN IP ADDR
			//			RETURN IP IF YES, RETURN DEFAULT OTHERWISE
				if (strHost.find('.') != strHost.npos)
					return (Network::sToIPV4Packed(strHost) );
				else
					return (Network::sToIPV4Packed(DEFAULT_HOST) );
			}
		}
	}

	return (Network::sToIPV4Packed(DEFAULT_HOST) );
}

uint16_t	ConfigParser::parsePort(std::vector<std::string> strServerBlock)
{
	std::string	strPort;
	size_t		vectorSize;
	int			nPort;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			strPort = str_parse_line(strServerBlock.at(i) );

			if (strPort.find(':') != strPort.npos)
				strPort = strPort.substr(strPort.find(':') + 1);
			else if (strPort.find('.') != strPort.npos)
				return (DEFAULT_PORT);

			nPort = std::atoi(strPort.c_str() );
			if (strPort.size() > 5 || nPort > 0xffff)
				throw (ExceptionMaker("Port number is out of range") );
			
			return (static_cast<uint16_t>(nPort) );
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
			return (str_parse_line(strServerBlock.at(i)) );
		}
	}

	return (DEFAULT_SERVER_NAME);
}


Utils::LOCATION_BLOCK_TYPE	ConfigParser::parseStrLocationType(std::vector<std::string> strLocationBlock)
{
	//	THIS FUNCTION WILL BE IMPLEMENTED LATER
	(void)strLocationBlock;
	return (Utils::L_STATIC);
}


/*		PARSING FOR SERVERLOCATION BASE CLASS		*/

std::string	ConfigParser::parseLocation(std::string locationLine)
{
	locationLine = str_parse_line(locationLine);

//	REPLACE THE SECOND CONDITIONAL WITH A FUNCTION FOR WORD COUNT
	if (!locationLine.empty() || word_count(locationLine) != 1)
		return (locationLine);
	else
		throw (ExceptionMaker("Invalid number of arguments in \"location\" directive") );
}

std::string	ConfigParser::parseRootDir(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strLocationBlock.at(i).find("root") == 0)
		{
				line = str_parse_line(line);
				if (word_count(line) > 1)
					throw (ExceptionMaker("Invalid number of arguments in \"root\" directive") );
				return (line);
		}
	}

	if (_defaultRoot.empty() )
		return (DEFAULT_ROOT);
	else
		return (_defaultRoot);
}

void	ConfigParser::parseIndexFiles(std::vector<std::string> strLocationBlock, std::vector<std::string> &indexFiles)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("index") == 0)
		{
			line = str_parse_line(line);
			split_string_to_vector(line, indexFiles);
		}
	}
	
	if (indexFiles.empty() )
	{
		if (_defaultIndex.empty() )
			indexFiles.push_back(DEFAULT_INDEX);
		else
		{
			for (size_t i = 0; i < _defaultIndex.size(); i++)
				indexFiles.push_back(_defaultIndex.at(i) );
		}
	}
}

uint32_t	ConfigParser::parseMaxBodySize(std::vector<std::string> strLocationBlock)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	long unsigned int	ulConvert;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("client_max_body_size") == 0)
		{
			while (++i < vectorSize)
			{
				if (strLocationBlock.at(i).find("client_max_body_size") == 0)
					throw (ExceptionMaker("\"client_max_body_size\" directive is duplicate") );
			}

			line = str_parse_line(line);
			if (word_count(line) > 1)
				throw (ExceptionMaker("Invalid number of arguments in \"client_max_body_size\" directive") );
			strStream << line;
			strStream >> ulConvert;
			if (line.size() > 10 || ulConvert > 0xffffffff)
				throw (ExceptionMaker("Value defined in \"client_max_body_size\" directive is too large") );
			else
				return (static_cast<uint32_t>(ulConvert) );
		}
	}

	return (DEFAULT_MAX_BODY_SIZE);	
}

void	ConfigParser::parseErrorPages(std::vector<std::string> strLocationBlock, IntStrMap &errorPages)
{
	size_t				vectorSize;
	std::string			line;
	int					errCode;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("error_page") == 0)
		{
			line = str_parse_line(line);
			if (word_count(line) != 2)
				throw (ExceptionMaker("Invalid number of arguments in \"error_page\" directive") );
			errCode = std::atoi(line.substr(0, line.find_first_of(" \t") ).c_str() );
			if (errorPages.find(errCode) != errorPages.end() )
				throw (ExceptionMaker("Multiple settings for the same error page in location context") );
			errorPages[errCode] = line.substr(line.find_last_of(" \t") + 1);
		}
	}


	if (errorPages.find(400) == errorPages.end() )
	{
		if (_defaultErrorPages.find(400) == _defaultErrorPages.end() )
			errorPages[400] = DEFAULT_400;
		else
			errorPages[400] = _defaultErrorPages[400];
	}

	if (errorPages.find(403) == errorPages.end() )
	{
		if (_defaultErrorPages.find(403) == _defaultErrorPages.end() )
			errorPages[403] = DEFAULT_403;
		else
			errorPages[403] = _defaultErrorPages[403];
	}

	if (errorPages.find(404) == errorPages.end() )
	{
		if (_defaultErrorPages.find(404) == _defaultErrorPages.end() )
			errorPages[404] = DEFAULT_404;
		else
			errorPages[404] = _defaultErrorPages[404];
	}

	if (errorPages.find(405) == errorPages.end() )
	{
		if (_defaultErrorPages.find(405) == _defaultErrorPages.end() )
			errorPages[405] = DEFAULT_405;
		else
			errorPages[405] = _defaultErrorPages[405];
	}

	if (errorPages.find(500) == errorPages.end() )
	{
		if (_defaultErrorPages.find(500) == _defaultErrorPages.end() )
			errorPages[500] = DEFAULT_500;
		else
			errorPages[500] = _defaultErrorPages[500];
	}

	if (errorPages.find(501) == errorPages.end() )
	{
		if (_defaultErrorPages.find(501) == _defaultErrorPages.end() )
			errorPages[501] = DEFAULT_501;
		else
			errorPages[501] = _defaultErrorPages[501];
	}
}

void	ConfigParser::parseRedirections(std::vector<std::string> strLocationBlock, StrStrMap &redirections)
{
	size_t		vectorSize;
	std::string	line;
	std::string	uri;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("rewrite") == 0)
		{
			line = str_parse_line(line);
			if (word_count(line) == 0)
				throw (ExceptionMaker("Invalid number of arguments in \"rewrite\" directive") );
			uri = line.substr(0, line.find_first_of(" \t") );
			if (redirections.find(uri) == redirections.end() )
				redirections[uri] = line.substr(line.find_last_of(" \t") );
			else
				throw (ExceptionMaker("Duplicate redirection provided in \"rewrite\" directives") );
		}
	}
}

void	ConfigParser::parseAllowedMethods(std::vector<std::string> strLocationBlock, std::vector<Http::METHOD> &methodsAllowed)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	std::string			method;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("allow_methods") == 0)
		{
			while (++i < vectorSize)
			{
				if (strLocationBlock.at(i).find("allow_methods") == 0)
					throw (ExceptionMaker("\"allow_methods\" directive is duplicate") );
			}

			line = str_parse_line(line);
			if (word_count(line) == 0)
				throw (ExceptionMaker("Invalid number of arguments in \"allow_methods\" directive") );

			strStream << line;
			while (strStream >> method)
			{
				if (method == "GET")
					methodsAllowed.push_back(Http::M_GET);
				else if (method == "POST")
					methodsAllowed.push_back(Http::M_POST);
				else if (method == "DELETE")
					methodsAllowed.push_back(Http::M_DELETE);
				else
					throw (ExceptionMaker("Invalid argument in \"allow_methods\" directive") );
			}
		}
	}

	if (methodsAllowed.empty() )
	{
		methodsAllowed.push_back(Http::M_GET);
	}
}

/*		PARSING FOR LOCATIONSTATIC DERIVED CLASS		*/

bool	ConfigParser::parseAutoIndex(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);	
		if (line.find("autoindex") == 0)
		{
			while (++i < vectorSize)
			{
				if (line.find("autoindex") == 0)
					throw (ExceptionMaker("\"autoindex\" directive is duplicate") );
			}

			line = str_parse_line(line);
			if (word_count(line) > 1)
				throw (ExceptionMaker("Invalid number of arguments in \"autoindex\" directive") );

			if (line == "on")
				return (true);
			else if (line == "off")
				return (false);
			else
				throw (ExceptionMaker("Invalid argument in \"autoindex\" directive") );
		}
	}

	return (DEFAULT_AUTO_INDEX);	
}
