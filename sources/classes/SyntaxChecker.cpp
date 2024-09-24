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

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i) == "{")
		{
			if (strServerBlock.at(i - 1).find("server") == 0)
				continue ;
			else
				throw (ExceptionMaker("Unexpected '{' token found") );
		}
		else if (strServerBlock.at(i) == "}")
			continue ;

		switch (_directiveCheck(strServerBlock.at(i) ) )
		{
			case(DIRECTIVE_SERVER):
				if (i != 0)
					throw (ExceptionMaker("Nested \"server\" directive found") );
				_syntaxCheckServer(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_LISTEN):
				_syntaxCheckListen(strServerBlock, i);
				break ;
			case(DIRECTIVE_SERVER_NAME):
				_syntaxCheckServerName(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_LOCATION):
				_syntaxCheckLocationBlock(strServerBlock, i);
				break ;
			case(DIRECTIVE_ROOT):
				//_syntaxCheckRoot(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_INDEX):
				//_syntaxCheckIndex(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_CLIENT_MAX_BODY_SIZE):
				//_syntaxCheckClientMaxBodySize(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_ERROR_PAGE):
				//_syntaxCheckErrorPage(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_REWRITE):
				//_syntaxCheckRewrite(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_ALLOW_METHODS):
				//_syntaxCheckAllowMethods(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_AUTOINDEX):
				//_syntaxCheckAutoIndex(strServerBlock, i, CONTEXT_SERVER);
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
		_strLocationBlock.push_back(strServerBlock.at(i));
		i++;
	}
	//	DO A SWITCH/CASE TO PARSE EACH DIRECTIVE UNTIL END OF LOCATION BLOCK
	vectorSize = _strLocationBlock.size();
	for (size_t j = 0; j < vectorSize; j++)
	{
		if (_strLocationBlock.at(j) == "{")
		{
			if (_strLocationBlock.at(j - 1).find("location") != 0)
				throw (ExceptionMaker("Unexpected '{' token found") );
			else
				continue ;
		}
		else if (_strLocationBlock.at(j) == "}")
			continue ;

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
//				_syntaxCheckLocationDirective(strServerBlock, i);
				break ;
			case(DIRECTIVE_ROOT):
				//_syntaxCheckRoot(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_INDEX):
				//_syntaxCheckIndex(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_CLIENT_MAX_BODY_SIZE):
				//_syntaxCheckClientMaxBodySize(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_ERROR_PAGE):
				//_syntaxCheckErrorPage(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_REWRITE):
				//_syntaxCheckRewrite(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_ALLOW_METHODS):
				//_syntaxCheckAllowMethods(strServerBlock, i, CONTEXT_SERVER);
				break ;
			case(DIRECTIVE_AUTOINDEX):
				//_syntaxCheckAutoIndex(strServerBlock, i, CONTEXT_SERVER);
				break ;
			default:
				throw (ExceptionMaker("Invalid directive in configuration file") );
		}
	}

	_strLocationBlock.clear();
}
