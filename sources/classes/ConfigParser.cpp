#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;

std::string					ConfigParser::_defaultRoot;
std::vector<std::string>	ConfigParser::_defaultIndex;
uint32_t					ConfigParser::_defaultMaxBodySize = DEFAULT_MAX_BODY_SIZE;
IntStrMap					ConfigParser::_defaultErrorPages;
StrStrMap					ConfigParser::_defaultRedirections;
std::vector<std::string>	ConfigParser::_defaultMethodsAllowed;
bool						ConfigParser::_defaultAutoIndex;


static void	erase_comments(std::string &line)
{
	size_t	pos;

	pos = line.find("#");
	if (pos != line.npos)
		line.erase(pos, line.size());
}

static void	split_string_to_vector(std::string line, std::vector<std::string> &strVector)
{
	std::istringstream	strStream(line);
	std::string			outStr;

	while(strStream >> outStr)
	{
		outStr = Utils::sTrim(outStr);
		strVector.push_back(outStr);
	}
}

void	ConfigParser::parseConfigs(const char *path, ServerBlocks &configs)
{
	std::ifstream	configFile;
	std::string		line;


	configFile.open(path);
	if (!configFile.is_open())
		throw (ExceptionMaker("Unable to open configuration file") );

	while (!configFile.eof() )
	{
		_loadServerContext(configFile);
		if (_strServerBlock.empty() )
			break ;

		SyntaxChecker::syntaxCheckServerBlock(_strServerBlock);

		_overrideDefaults();

		configs.push_back(ServerConfig(_strServerBlock) );	

		_strServerBlock.clear();

		_defaultRoot.clear();
		_defaultIndex.clear();
		_defaultMaxBodySize = DEFAULT_MAX_BODY_SIZE;
		_defaultErrorPages.clear();
		_defaultRedirections.clear();
		_defaultMethodsAllowed.clear();
		_defaultAutoIndex = DEFAULT_AUTO_INDEX;
	}

	configFile.close();
	if (configs.empty() )
		throw (ExceptionMaker("Configuration file is empty") );


//	THIS IS FOR DEBUGGING ONLY
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

	while (std::getline(configFile, line) )
	{
		erase_comments(line);
		line = Utils::sTrim(line);

		if (line.empty() )
			continue ;
		else if (line.find("server") == 0)
		{
			_strServerBlock.push_back(line);
			if (line.at(line.size() - 1) != '{')
			{
				do
				{
					std::getline(configFile, line);
					line = Utils::sTrim(line);
				} while (line == "");

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
		
		if (brackets == 0)
			break ;
	}

	if (brackets != 0)
		throw (ExceptionMaker("Syntax error in Server context encapsulation") );
}


void	ConfigParser::_overrideDefaults(void)
{
	size_t				vectorSize;
	int					nEdgeStatusCode;
	uint32_t			unMaxBodySize;
	std::stringstream	strStream;
	std::string			line;

	vectorSize = _strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = _strServerBlock.at(i);

		if (line.find("location") == 0)
		{
			while (line.find('}') == line.npos)
			{
				i++;
				line = _strServerBlock.at(i);
			}
		}

		if (line.find("root") == 0)
			_defaultRoot = SyntaxChecker::strParseLine(line);
		else if (line.find("index") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			split_string_to_vector(line, _defaultIndex);
		}
		else if (line.find("client_max_body_size") == 0)
		{
			line = SyntaxChecker::strParseLine(line);

			strStream << line;
			strStream >> unMaxBodySize;
			_defaultMaxBodySize = unMaxBodySize;
		}
		else if (line.find("error_page") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			nEdgeStatusCode = std::atoi(line.substr(0, line.find_first_of(" \t") ).c_str() );
			if (_defaultErrorPages.find(nEdgeStatusCode) == _defaultErrorPages.end() )
				_defaultErrorPages[nEdgeStatusCode] = line.substr(line.find_last_of(" \t") + 1);
			else
				throw (ExceptionMaker("Multiple error pages for the same edge status code in server context") );
		}
		else if (line.find("rewrite") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (_defaultRedirections.find(line.substr(0, line.find_first_of(" \t") ) ) == _defaultRedirections.end() )
				_defaultRedirections[line.substr(0, line.find_first_of(" \t") )] = Utils::sTrim(line.substr(line.find_last_of(" \t") ) );
			else
				throw (ExceptionMaker("Multiple redirections for the same url in server context") );
		}
		else if (line.find("allow_methods") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (_defaultMethodsAllowed.empty() )
				split_string_to_vector(line, _defaultMethodsAllowed);
			else
				throw (ExceptionMaker("Multiple overrides for default \"allow_methods\" directive inside same server context") );
		}
		else if (line.find("autoindex") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (line == "on")
				_defaultAutoIndex = true;
			else if (line == "off")
				_defaultAutoIndex = false;
			else
				throw (ExceptionMaker("Invalid argument in \"autoindex\" directive in server context") );

		}
	}
}


uint32_t	ConfigParser::parseHost(std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	strHost;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			strHost = SyntaxChecker::strParseLine(strServerBlock.at(i) );

			if (strHost.find(':') != strHost.npos)
				return (Network::sToIPV4Packed(strHost.substr(0, strHost.find(':') ) ) );
			else
			{
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
			strPort = SyntaxChecker::strParseLine(strServerBlock.at(i) );

			if (strPort.find(':') != strPort.npos)
				strPort = strPort.substr(strPort.find(':') + 1);
			else if (strPort.find('.') != strPort.npos)
				return (DEFAULT_PORT);

			nPort = std::atoi(strPort.c_str() );
			return (static_cast<uint16_t>(nPort) );
		}
	}

	return (DEFAULT_PORT);
}

void	ConfigParser::parseServerName(std::vector<std::string> strServerBlock, std::vector<std::string> &serverNames)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	std::string			strArg;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strServerBlock.at(i);
		if (line.find("server_name") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strStream << line;
			while (strStream >> strArg)
			{
				if (std::find(serverNames.begin(), serverNames.end(), strArg) != serverNames.end() )
					throw (ExceptionMaker("Multiple instances of the same argument found in \"server_name\" directive") );
				serverNames.push_back(strArg);
			}
			strStream.clear();
		}
	}
}


Utils::LOCATION_BLOCK_TYPE	ConfigParser::parseStrLocationType(std::vector<std::string> strLocationBlock)
{
	//	THIS FUNCTION WILL BE IMPLEMENTED LATER
	(void)strLocationBlock;
	return (Utils::L_STATIC);
}


std::string	ConfigParser::parseLocation(std::string locationLine)
{
	locationLine = SyntaxChecker::strParseLine(locationLine);

	if (!locationLine.empty() || Utils::sWordCount(locationLine) != 1)
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
		line = strLocationBlock.at(i);
		if (line.find("root") == 0)
		{
				line = SyntaxChecker::strParseLine(line);
				if (Utils::sWordCount(line) > 1)
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
			line = SyntaxChecker::strParseLine(line);
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
	uint32_t			unMaxBodySize;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("client_max_body_size") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strStream << line;
			strStream >> unMaxBodySize;
			return (unMaxBodySize);
		}
	}

	return (_defaultMaxBodySize);	
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
			line = SyntaxChecker::strParseLine(line);
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
			line = SyntaxChecker::strParseLine(line);
			uri = line.substr(0, line.find_first_of(" \t") );
			if (redirections.find(uri) == redirections.end() )
				redirections[uri] = Utils::sTrim(line.substr(line.find_last_of(" \t") ) );
			else
				throw (ExceptionMaker("Duplicate redirection provided in \"rewrite\" directives") );
		}
	}

	for (StrStrMap::iterator itt = _defaultRedirections.begin(); itt != _defaultRedirections.end(); itt++)
	{
		if (redirections.find(itt->first) == redirections.end() )
			redirections[itt->first] = itt->second;
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
			line = SyntaxChecker::strParseLine(line);
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
		if (_defaultMethodsAllowed.empty() )
			methodsAllowed.push_back(Http::M_GET);
		else
		{
			for (size_t i = 0; i < _defaultMethodsAllowed.size(); i++)
			{
				if (_defaultMethodsAllowed.at(i) == "GET")
					methodsAllowed.push_back(Http::M_GET);
				else if (_defaultMethodsAllowed.at(i) == "POST")
					methodsAllowed.push_back(Http::M_POST);
				else if (_defaultMethodsAllowed.at(i) == "DELETE")
					methodsAllowed.push_back(Http::M_DELETE);
				else
					throw (ExceptionMaker("Invalid argument in \"allow_methods\" directive in server context") );
			}
		}
	}
}


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
				if (strLocationBlock.at(i).find("autoindex") == 0)
					throw (ExceptionMaker("\"autoindex\" directive is duplicate") );
			}

			line = SyntaxChecker::strParseLine(line);
			if (Utils::sWordCount(line) > 1)
				throw (ExceptionMaker("Invalid number of arguments in \"autoindex\" directive") );

			if (line == "on")
				return (true);
			else if (line == "off")
				return (false);
			else
				throw (ExceptionMaker("Invalid argument in \"autoindex\" directive") );
		}
	}

	return (_defaultMaxBodySize);	
}
