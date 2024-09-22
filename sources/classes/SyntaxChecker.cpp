#include "../../includes/classes/SyntaxChecker.hpp"


SyntaxChecker::syntaxCheckServerBlock(std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strServerBlock.size();
	fot (size_t i = 0; i < vectorSize; i++)
	{
		line = strServerBlock.at(i);

		if (line != "{" && line != "}")
			//	GET DIRECTIVE FROM LINE (NUMBER CODED)

		//	DO A SWITCH/CASE TO DETERMINE WHICH DIRECTIVE
		//	CALL FUNCTION TO CHECK CORRESPONDING DIRECTIVE
	}
}