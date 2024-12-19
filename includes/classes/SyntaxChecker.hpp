#pragma once

#include "../webserv.hpp"

typedef enum e_directive
{
	DIRECTIVE_SERVER,
	DIRECTIVE_LISTEN,
	DIRECTIVE_SERVER_NAME,
	DIRECTIVE_LOCATION,
	DIRECTIVE_ROOT,
	DIRECTIVE_INDEX,
	DIRECTIVE_CLIENT_MAX_BODY_SIZE,
	DIRECTIVE_ERROR_PAGE,
	DIRECTIVE_REWRITE,
	DIRECTIVE_ALLOW_METHODS,
	DIRECTIVE_UPLOAD_STORE,
	DIRECTIVE_AUTOINDEX,
	DIRECTIVE_CGI_EXTENSION
}	t_directive;

class	SyntaxChecker
{
	public:
		static void			syntaxCheckServerBlock(const StrArr strServerBlock);

		static std::string	strParseLine(std::string line);


	private:
		SyntaxChecker(void);
		~SyntaxChecker(void);

		static int	_directiveCheck(const std::string line);

		static void	_syntaxCheckServer(const std::string line);
		static void	_syntaxCheckListen(const StrArr strServerBlock, const size_t i);
		static void	_syntaxCheckServerName(const std::string line);
		static void	_syntaxCheckLocationBlock(const StrArr strServerBlock, size_t &i);
		static void	_syntaxCheckLocationDirective(const std::string line);
		static void	_syntaxCheckRoot(const StrArr block, const size_t i);
		static void	_syntaxCheckIndex(const std::string line);
		static void _syntaxCheckClientMaxBodySize(const StrArr block, const size_t i);
		static void _syntaxCheckErrorPage(const std::string line);
		static void	_syntaxCheckRewrite(const std::string line);
		static void	_syntaxCheckAllowMethods(const StrArr block, const size_t i);
		static void	_syntaxCheckUploadStore(const StrArr block, const size_t i);
		static void	_syntaxCheckAutoIndex(const StrArr block, const size_t i);
		static void	_syntaxCheckCgiExtension(std::string line);

		static StrArr	_strLocationBlock;
		static const std::string		_directivesList[];
};