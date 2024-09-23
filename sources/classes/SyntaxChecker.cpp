#include "../../includes/classes/SyntaxChecker.hpp"


std::vector<std::string>	SyntaxChecker::_strLocationBlock;


void	SyntaxChecker::syntaxCheckServerBlock(std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strServerBlock.at(i);
		if (line == "{" || line == "}")
			continue ;

		//	GET DIRECTIVE FROM LINE (NUMBER CODED)
		switch(_directiveCheck(line) )
		{
			case(DIRECTIVE_SERVER):
				if (i != 0)
					throw (ExceptionMaker("nested \"server\" directive found") );

				break ;
			case(DIRECTIVE_LISTEN):

				break ;
			case(DIRECTIVE_SERVER_NAME):

				break ;
			case(DIRECTIVE_LOCATION):

				break ;
			case(DIRECTIVE_ROOT):

				break ;
			case(DIRECTIVE_INDEX):

				break ;
			case(DIRECTIVE_CLIENT_MAX_BODY_SIZE):

				break ;
			case(DIRECTIVE_ERROR_PAGE):

				break ;
			case(DIRECTIVE_REWRITE):

				break ;
			case(DIRECTIVE_ALLOW_METHODS):

				break ;
			case(DIRECTIVE_AUTOINDEX):

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