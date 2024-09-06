#include "../../includes/classes/ConfigParser.hpp"

std::vector<std::string>	ConfigParser::_strServerBlock;
std::string					ConfigParser::_defaultRoot;
std::string					ConfigParser::_defaultIndex;


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

		_overrideDefaults();

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
		

		if (line.find("server ") == 0)
			throw (ExceptionMaker("Syntax error in Server context encapsulation") );
		else if (brackets == 0)
			break ;
	}

	if (brackets != 0)
		throw (ExceptionMaker("Syntax error in Server context encapsulation") );
}

void	ConfigParser::_overrideDefaults(void)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = _strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = _strServerBlock.at(i);
		if (line.find("location") == 0)
		{
			while (line.at(line.size() - 1) != '}')
			{
				i++;
				line = _strServerBlock.at(i);
			}
		}

		if (line.find("root") == 0)
		{
			if (_defaultRoot.empty() )
			{
				line.erase(0, 4);
				line.erase(line.size() - 1, line.size());
				line = Utils::sTrim(line);
				_defaultRoot = line;
			}
			else
				throw (ExceptionMaker("Multiple overrides for default root directive inside same server context") );
		}
		else if (line.find("index") == 0)
		{
			if (_defaultIndex.empty() )
			{
				line.erase(0, 5);
				line.erase(line.size() - 1, line.size());
				line = Utils::sTrim(line);
				_defaultIndex = line;
			}
			else
				throw (ExceptionMaker("Multiple overrides for default index directive inside same server context") );
		}
	}
}
