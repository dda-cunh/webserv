#pragma once

#include "../webserv.hpp"


class	SyntaxChecker
{
	public:
		static void	syntaxCheckServerBlock(std::vector<std::string> strServerBlock);

	private:
		SyntaxChecker(void);
		~SyntaxChecker(void);

		static std::vector<std::string>	_strLocationBlock;
};