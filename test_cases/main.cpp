#include "ConfigParser.hpp"

static void	displayHelp(void)
{
	std::cout << "Usage:" << std::endl;
	std::cout << "./test [config file]" << std::endl;
}

int	main(int argc, char **argv)
{
	std::string	fileName;
	if (argc == 2)
	{
		fileName = argv[1];
		ConfigParser::parseConfigs(fileName);
	}
	else
		displayHelp();
	return (0);
}

