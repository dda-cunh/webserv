#include "ConfigParser.hpp"

static void	displayHelp(void)
{
	std::cout << "Usage:" << std::endl;
	std::cout << "./test [config file]" << std::endl;
}

int	main(int argc, char **argv)
{
	std::string	fileName;
	switch (argc)
	{
		case (1):
			fileName = "configs/inception";
			ConfigParser::parseConfigs(fileName);
			break ;
		case (2):
			fileName = argv[1];
			ConfigParser::parseConfigs(fileName);
			break ;
		default:
			displayHelp();
	}
	return (0);
}

