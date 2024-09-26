#include "../../includes/classes/SyntaxChecker.hpp"

std::vector<std::string>	SyntaxChecker::_strLocationBlock;
const std::string			SyntaxChecker::_directivesList[] = {"server", "listen", "server_name", "location",\
																"root", "index", "client_max_body_size", "error_page",\
																"rewrite", "allow_methods", "autoindex",\
																""};


std::string	SyntaxChecker::strParseLine(std::string line)
{
	std::string	val;

	val = line;

	val.erase(0, val.find_first_of(" \t") );
	if (val.find_first_of("{;") != val.npos)
		val.erase(val.find_first_of(";{"));
	val = Utils::sTrim(val);

	return (val);
}


void	SyntaxChecker::syntaxCheckServerBlock(const std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strServerBlock.at(i);

		if (line == "{")
		{
			if (strServerBlock.at(i - 1).find("server") == 0)
				continue ;
			else
				throw (ExceptionMaker("Unexpected '{' token found") );
		}
		else if (line == "}")
			continue ;

		switch (_directiveCheck(line) )
		{
			case(DIRECTIVE_SERVER):
				if (i != 0)
					throw (ExceptionMaker("Nested \"server\" directive found") );
				_syntaxCheckServer(line);
				break ;
			case(DIRECTIVE_LISTEN):
				_syntaxCheckListen(strServerBlock, i);
				break ;
			case(DIRECTIVE_SERVER_NAME):
				_syntaxCheckServerName(line);
				break ;
			case(DIRECTIVE_LOCATION):
				if (line.at(line.size() - 1) != '{' && strServerBlock.at(i + 1) != "{")
					throw (ExceptionMaker("Directive \"location\" has no opening \"{\"") );
				else if (line.at(line.size() - 1) == '{' && strServerBlock.at(i + 1) == "{")
					throw (ExceptionMaker("Unexpected '{' token found") );
				_syntaxCheckLocationBlock(strServerBlock, i);
				break ;
			case(DIRECTIVE_ROOT):
				_syntaxCheckRoot(strServerBlock, i);
				break ;
			case(DIRECTIVE_INDEX):
				_syntaxCheckIndex(line);
				break ;
			case(DIRECTIVE_CLIENT_MAX_BODY_SIZE):
				_syntaxCheckClientMaxBodySize(strServerBlock, i);
				break ;
			case(DIRECTIVE_ERROR_PAGE):
				_syntaxCheckErrorPage(line);
				break ;
			case(DIRECTIVE_REWRITE):
				_syntaxCheckRewrite(line);
				break ;
			case(DIRECTIVE_ALLOW_METHODS):
				_syntaxCheckAllowMethods(strServerBlock, i);
				break ;
			case(DIRECTIVE_AUTOINDEX):
				//_syntaxCheckAutoIndex(strServerBlock, i);
				break ;
			default:
				throw (ExceptionMaker("Invalid directive in configuration file") );
		}
	}
}

int	SyntaxChecker::_directiveCheck(const std::string line)
{
	std::string					directive;

	directive = line.substr(0, line.find_first_of(" \t;{") );
	for (int i = 0; _directivesList[i] != ""; i++)
	{
		if (directive == _directivesList[i])
			return (i);
	}

	return (-1);
}


void	SyntaxChecker::_syntaxCheckServer(const std::string line)
{
	if (line != "server" && (line.find_first_of("{") != line.size() - 1 || line.find(';') != line.npos) )
		throw (ExceptionMaker("Unexpected tokens in \"server\" directive") );

	if (strParseLine(line) != "")
		throw (ExceptionMaker("Invalid number of arguments in \"server\" directive") );
}


static void	syntax_check_IP(std::string strIP)
{
	std::string	strOctet;

	for (int i = 0; i < 4; i++)
	{
		if (strIP.empty() )
			throw (ExceptionMaker("Invalid IP provided in \"listen\" directive") );
		
		strOctet = strIP.substr(0, strIP.find('.') );

		for (size_t j = 0; j < strOctet.size(); j++)
		{
			if (!std::isdigit(strOctet.at(j) ) )
				throw (ExceptionMaker("Invalid IP provided in \"listen\" directive") );
		}

		if (strOctet.size() > 3 || std::atoi(strOctet.c_str() ) > 255)
			throw (ExceptionMaker("Invalid IP provided in \"listen\" directive") );

		if (i < 3 && strIP.find('.') == strIP.npos)
			throw (ExceptionMaker("Invalid IP provided in \"listen\" directive") );
		
		strIP.erase(0, strIP.find('.') + 1);
	}
}

static void	syntax_check_port(std::string strPort)
{
	for (size_t i = 0; i < strPort.size(); i++)
	{
		if (!std::isdigit(strPort.at(i) ) )
			throw (ExceptionMaker("Invalid port provided in \"listen\" directive") );
	}

	if (strPort.size() > 5 || std::atoi(strPort.c_str() ) > 0xffff)
		throw (ExceptionMaker("Port provided in \"listen\" directive is out of range") );
}

void	SyntaxChecker::_syntaxCheckListen(const std::vector<std::string> strServerBlock, const size_t i)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strServerBlock.size();
	for (size_t j = i + 1; j < vectorSize; j++)
	{
		if (strServerBlock.at(j).find("listen") == 0)
			throw (ExceptionMaker("\"listen\" directive is duplicate") );
	}

	line = strServerBlock.at(i);
	if (line.find_first_of(";") != line.size() - 1)
		throw (ExceptionMaker("Expected ';' token at the end of \"listen\" directive") );

	line = strParseLine(line);
	if (Utils::sWordCount(line) != 1)
		throw (ExceptionMaker("Incorrect number of arguments in \"listen\" directive") );

	if (line.find(':') != line.npos)
	{
		syntax_check_IP(line.substr(0, line.find(':') ) );
		syntax_check_port(line.substr(line.find(':') + 1) );
	}
	else if (line.find('.') != line.npos)
		syntax_check_IP(line);
	else
		syntax_check_port(line);
}

void	SyntaxChecker::_syntaxCheckServerName(const std::string line)
{
	if (line.find_first_of(";") != line.size() - 1)
		throw (ExceptionMaker("Expected ';' token at the end of \"server_name\" directive") );

	if (Utils::sWordCount(strParseLine(line) ) == 0)
		throw (ExceptionMaker("Invalid number of arguments in \"server_name\" directive") );
}

void	SyntaxChecker::_syntaxCheckLocationBlock(const std::vector<std::string> strServerBlock, size_t &i)
{
	size_t	vectorSize;

	while (strServerBlock.at(i) != "}")
	{
		_strLocationBlock.push_back(strServerBlock.at(i) );
		i++;
	}
	_strLocationBlock.push_back(strServerBlock.at(i) );

	vectorSize = _strLocationBlock.size();
	for (size_t j = 0; j < vectorSize; j++)
	{
		if (_strLocationBlock.at(j).find('{') != _strLocationBlock.at(j).npos)
		{
			if (j > 1 || \
				(j == 1 && _strLocationBlock.at(j - 1).find('{') != _strLocationBlock.at(j - 1).npos) )
				throw (ExceptionMaker("Unexpected '{' token found") );
			else
				continue ;
		}
		else if (_strLocationBlock.at(j).find('}') != _strLocationBlock.at(j).npos)
		{
			if (_strLocationBlock.at(j) == "}")
				continue ;
			else
				throw (ExceptionMaker("Unexpected '}' token found") );
		}

		switch (_directiveCheck(_strLocationBlock.at(j) ) )
		{
			case(DIRECTIVE_SERVER):
				throw (ExceptionMaker("\"server\" directive is not allowed in Location context") );
				break ;
			case(DIRECTIVE_LISTEN):
				throw (ExceptionMaker("\"listen\" directive is not allowed in Location context") );
				break ;
			case(DIRECTIVE_SERVER_NAME):
				throw (ExceptionMaker("\"server_name\" directive is not allowed in Location context") );
				break ;
			case(DIRECTIVE_LOCATION):
				if (j != 0)
					throw (ExceptionMaker("Nested \"location\" directive found") );
				_syntaxCheckLocationDirective(_strLocationBlock.at(j) );
				break ;
			case(DIRECTIVE_ROOT):
				_syntaxCheckRoot(_strLocationBlock, j);
				break ;
			case(DIRECTIVE_INDEX):
				_syntaxCheckIndex(_strLocationBlock.at(j) );
				break ;
			case(DIRECTIVE_CLIENT_MAX_BODY_SIZE):
				_syntaxCheckClientMaxBodySize(_strLocationBlock, j);
				break ;
			case(DIRECTIVE_ERROR_PAGE):
				_syntaxCheckErrorPage(_strLocationBlock.at(j) );
				break ;
			case(DIRECTIVE_REWRITE):
				_syntaxCheckRewrite(_strLocationBlock.at(j) );
				break ;
			case(DIRECTIVE_ALLOW_METHODS):
				_syntaxCheckAllowMethods(_strLocationBlock, j);
				break ;
			case(DIRECTIVE_AUTOINDEX):
				//_syntaxCheckAutoIndex(_strLocationBlock, j);
				break ;
			default:
				throw (ExceptionMaker("Invalid directive in configuration file") );
		}
	}

	//	DO A CHECK TO THROW ERROR IF BLOCK HAS MIXED DIRECTIVES FOR DIFFERENT TYPE OF LOCATION OBJ

	_strLocationBlock.clear();
}

void	SyntaxChecker::_syntaxCheckLocationDirective(const std::string line)
{
	if (line.find(';') != line.npos)
		throw (ExceptionMaker("Unexpected \";\" found in \"location\" directive") );
	else if (Utils::sWordCount(strParseLine(line) ) != 1)
		throw (ExceptionMaker("Incorrect number of arguments in \"location\" directive") );
}

void	SyntaxChecker::_syntaxCheckRoot(const std::vector<std::string> block, const size_t i)
{
	size_t	vectorSize;

	vectorSize = block.size();
	for (size_t j = i + 1; j < vectorSize; j++)
	{
		if (block.at(j).find("location") == 0)
		{
			while (j < vectorSize && block.at(j) != "}")
				j++;
		}

		if (block.at(j).find("root") == 0)
			throw (ExceptionMaker("\"root\" directive is duplicate") );
	}

	if (block.at(i).find(';') != block.at(i).size() - 1)
		throw (ExceptionMaker("Expected ';' token at the end of \"root\" directive") );

	if (Utils::sWordCount(strParseLine(block.at(i) ) ) != 1)
		throw (ExceptionMaker("Invalid number of arguments in \"root\" directive") );
}

void	SyntaxChecker::_syntaxCheckIndex(const std::string line)
{
	if (Utils::sWordCount(strParseLine(line) ) < 1)
		throw (ExceptionMaker("Invalid number of arguments in \"index\" directive") );
	if (line.find(';') != line.size() - 1)
		throw (ExceptionMaker("Expected ';' token at the end of \"index\" directive") );
}

void	SyntaxChecker::_syntaxCheckClientMaxBodySize(const std::vector<std::string> block, const size_t i)
{
	size_t				vectorSize;
	std::string			strArg;
	std::stringstream	strStream;
	unsigned long int	ulVal;

	vectorSize = block.size();
	for (size_t j = i + 1; j < vectorSize; j++)
	{
		if (block.at(j).find("location") == 0)
		{
			while (j < vectorSize && block.at(j) != "}")
				j++;
		}

		if (block.at(j).find("client_max_body_size") == 0)
			throw (ExceptionMaker("\"client_max_body_size\" directive is duplicate") );
	}

	if (block.at(i).find(';') != block.at(i).size() - 1)
		throw (ExceptionMaker("Expected ';' token at the end of \"client_max_body_size\" directive") );

	strArg = strParseLine(block.at(i) );
	if (Utils::sWordCount(strArg) != 1)
		throw (ExceptionMaker("Invalid number of arguments in \"client_max_body_size\" directive") );

	for (size_t j = 0; j < strArg.size(); j++)
	{
		if (!std::isdigit(strArg.at(j) ) )
			throw (ExceptionMaker("Argument provided in \"client_max_body_size\" directive must be numeric") );
	}

	strStream << strArg;
	strStream >> ulVal;
	if (strArg.size() > 10 || ulVal > 0xffffffff)
		throw (ExceptionMaker("Value provided as argument in \"client_max_body_size\" is out of range") );
}

void	SyntaxChecker::_syntaxCheckErrorPage(const std::string line)
{
	std::string	strArgs;
	int			nStatus;

	if (line.find(';') == line.npos)
		throw (ExceptionMaker("Expected ';' token at the end of \"error_page\" directive") );
	else if (line.find(';') != line.size() - 1)
		throw (ExceptionMaker("Unexpected ';' token found in  \"error_page\" directive") );

	strArgs = strParseLine(line);
	if (Utils::sWordCount(strArgs) != 2)
		throw (ExceptionMaker("Invalid number of arguments in \"error_page\" directive") );

	strArgs.erase(strArgs.find_first_of(" \t") );
	for (size_t i = 0; i < strArgs.size(); i++)
	{
		if (!std::isdigit(strArgs.at(i) ) )
			throw (ExceptionMaker("Status code provided in \"error_page\" directive must be numeric") );
	}

	nStatus = std::atoi(strArgs.c_str() );
	if (strArgs.size() != 3 || nStatus < 300 || nStatus > 599)
		throw (ExceptionMaker("Status code in \"error_page\" directive must be between 300 and 599") );
}

void	SyntaxChecker::_syntaxCheckRewrite(const std::string line)
{
	std::string	strArgs;

	if (line.find(';') == line.npos)
		throw (ExceptionMaker("Expected ';' token at the end of \"rewrite\" directive") );
	else if (line.find(';') != line.size() - 1)
		throw (ExceptionMaker("Unexpected ';' token found in  \"rewrite\" directive") );

	strArgs = strParseLine(line);
	if (Utils::sWordCount(strArgs) != 2)
		throw (ExceptionMaker("Invalid number of arguments in \"rewrite\" directive") );
}

void	SyntaxChecker::_syntaxCheckAllowMethods(const std::vector<std::string> block, const size_t i)
{
	size_t	vectorSize;
	size_t	nArgWords;

	vectorSize = block.size();
	for (size_t j = i + 1; j < vectorSize; j++)
	{
		if (block.at(j).find("location") == 0)
		{
			while (j < vectorSize && block.at(j) != "}")
				j++;
		}

		if (block.at(j).find("allow_methods") == 0)
			throw (ExceptionMaker("\"allow_methods\" directive is duplicate") );
	}

	if (block.at(i).find(';') == block.at(i).npos)
		throw (ExceptionMaker("Expected ';' token at the end of \"allow_methods\" directive") );
	else if (block.at(i).find(';') != block.at(i).size() - 1)
		throw (ExceptionMaker("Unexpected ';' token found in  \"allow_methods\" directive") );

	nArgWords = Utils::sWordCount(strParseLine(block.at(i) ) );
	if (nArgWords == 0 || nArgWords > 3)
		throw (ExceptionMaker("Invalid number of arguments in \"allow_methods\" directive") );
}