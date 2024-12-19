#include <iostream>

#include "../includes/classes/ServerManager.hpp"
#include "../includes/classes/ConfigParser.hpp"
#include "../includes/classes/ExceptionMaker.hpp"
#include "../includes/webserv.hpp"

int	main(int ac, char **av)
{
	ServerManager::ServerBlocks	blocks;
	std::string					configPath;

	if (ac == 2)
		configPath = av[1];
	else if (ac == 1)
		configPath = DEFAULT_CONFIG_PATH;
	else
	{
		std::cerr << "Usage: ./webserv [config_file]" << '\n';
		return (1);
	}

	try
	{
		ConfigParser::parseConfigs(configPath.c_str(), blocks);
	}
	catch (ExceptionMaker const &exception)
	{
		std::cerr << exception.what() << '\n';
		return (1);
	}

	ServerManager	server(blocks);
	return (0);
}
