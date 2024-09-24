#pragma once

#include "../webserv.hpp"


#define DIRECTIVE_SERVER 0
#define DIRECTIVE_LISTEN 1
#define DIRECTIVE_SERVER_NAME 2
#define DIRECTIVE_LOCATION 3
#define DIRECTIVE_ROOT 4
#define DIRECTIVE_INDEX 5
#define DIRECTIVE_CLIENT_MAX_BODY_SIZE 6
#define DIRECTIVE_ERROR_PAGE 7
#define DIRECTIVE_REWRITE 8
#define DIRECTIVE_ALLOW_METHODS 9
#define DIRECTIVE_AUTOINDEX 10

#define CONTEXT_SERVER 0
#define CONTEXT_LOCATION 1


class	SyntaxChecker
{
	public:
		static void			syntaxCheckServerBlock(const std::vector<std::string> strServerBlock);

		static std::string	strParseLine(std::string line);

	private:
		SyntaxChecker(void);
		~SyntaxChecker(void);

		static int	_directiveCheck(const std::string line);

		static void	_syntaxCheckServer(const std::string line);
		static void	_syntaxCheckListen(const std::vector<std::string> strServerBlock, const size_t i);
		static void	_syntaxCheckServerName(const std::string line);
		static void	_syntaxCheckLocationBlock(const std::vector<std::string> strServerBlock, size_t &i);

		static std::vector<std::string>	_strLocationBlock;
		static const std::string		_directivesList[];
};