#include "../../includes/classes/SyntaxChecker.hpp"


std::vector<std::string>	SyntaxChecker::_strLocationBlock;


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
		if (strServerBlock.at(i) == "{" || strServerBlock.at(i) == "}")
			continue ;

		switch(_directiveCheck(strServerBlock.at(i)) )
		{
			case(DIRECTIVE_SERVER):
				if (i != 0)
					throw (ExceptionMaker("Nested \"server\" directive found") );
				if (strServerBlock.at(i).at(strServerBlock.at(i).size() - 1) == '{' \
					&& strServerBlock.at(i + 1) == "{")
					throw (ExceptionMaker("Unexpected \"{\" token found") );
				_syntaxCheckServer(strServerBlock.at(i) );
				break ;
			case(DIRECTIVE_LISTEN):
				//_syntaxCheckListen(strServerBlock, i);
				break ;
			case(DIRECTIVE_SERVER_NAME):
				//_syntaxCheckServerName(strServerBlock, i);
				break ;
			case(DIRECTIVE_LOCATION):
				//_syntaxCheckLocation(strServerBlock, i);
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
	static const std::string	directivesList[] = {"server", "listen", "server_name", "location",\
													"root", "index", "client_max_body_size", "error_page",\
													"rewrite", "allow_methods", "autoindex",\
													""};
	std::string					directive;

	directive = line.substr(0, line.find_first_of(" \t;{") );
	for (int i = 0; directivesList[i] != ""; i++)
	{
		if (directive == directivesList[i])
			return (i);
	}

	return (-1);
}

void	SyntaxChecker::_syntaxCheckServer(const std::string line)
{
	std::cout << line << std::endl;
	if (line != "server" && (line.find_first_of("{") != line.size() - 1 || line.find(';') != line.npos) )
		throw (ExceptionMaker("Unexpected tokens in \"server\" directive") );

	if (strParseLine(line) != "")
		throw (ExceptionMaker("Invalid number of arguments in \"server\" directive") );
}