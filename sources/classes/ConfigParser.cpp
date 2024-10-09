#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;

std::string					ConfigParser::defaultRoot;
std::vector<std::string>	ConfigParser::defaultIndex;
uint32_t					ConfigParser::defaultMaxBodySize = DEFAULT_MAX_BODY_SIZE;
IntStrMap					ConfigParser::defaultErrorPages;
StrStrMap					ConfigParser::defaultRedirections;
std::vector<std::string>	ConfigParser::defaultMethodsAllowed;
std::string					ConfigParser::defaultUploadPath;


bool						ConfigParser::defaultAutoIndex;
//std::string					ConfigParser::defaultCgiPass;


static void	erase_comments(std::string &line)
{
	size_t	pos;

	pos = line.find("#");
	if (pos != line.npos)
		line.erase(pos, line.size());
}


void	ConfigParser::strToVecSplit(std::string line, std::vector<std::string> &strVector)
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

		defaultRoot.clear();
		defaultIndex.clear();
		defaultMaxBodySize = DEFAULT_MAX_BODY_SIZE;
		defaultErrorPages.clear();
		defaultRedirections.clear();
		defaultMethodsAllowed.clear();
		defaultAutoIndex = DEFAULT_AUTO_INDEX;
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

			defaultRoot = SyntaxChecker::strParseLine(line);
		else if (line.find("index") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strToVecSplit(line, defaultIndex);
		}
		else if (line.find("client_max_body_size") == 0)
		{
			line = SyntaxChecker::strParseLine(line);

			strStream << line;
			strStream >> unMaxBodySize;
			defaultMaxBodySize = unMaxBodySize;
		}
		else if (line.find("error_page") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			nEdgeStatusCode = std::atoi(line.substr(0, line.find_first_of(" \t") ).c_str() );
			if (defaultErrorPages.find(nEdgeStatusCode) == defaultErrorPages.end() )
				defaultErrorPages[nEdgeStatusCode] = line.substr(line.find_last_of(" \t") + 1);
			else
				throw (ExceptionMaker("Multiple error pages for the same edge status code in server context") );
		}
		else if (line.find("rewrite") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (defaultRedirections.find(line.substr(0, line.find_first_of(" \t") ) ) == defaultRedirections.end() )
				defaultRedirections[line.substr(0, line.find_first_of(" \t") )] = Utils::sTrim(line.substr(line.find_last_of(" \t") ) );
			else
				throw (ExceptionMaker("Multiple redirections for the same url in server context") );
		}
		else if (line.find("allow_methods") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (defaultMethodsAllowed.empty() )
				strToVecSplit(line, defaultMethodsAllowed);
			else
				throw (ExceptionMaker("Multiple overrides for default \"allow_methods\" directive inside same server context") );
		}
		else if (line.find("upload_store") == 0)
			defaultUploadPath = SyntaxChecker::strParseLine(line);
		else if (line.find("autoindex") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			if (line == "on")
				defaultAutoIndex = true;
			else if (line == "off")
				defaultAutoIndex = false;
			else
				throw (ExceptionMaker("Invalid argument in \"autoindex\" directive in server context") );
		}
	}
}
