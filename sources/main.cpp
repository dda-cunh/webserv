#include "../includes/classes/ServerManager.hpp"
#include "../includes/classes/ConfigParser.hpp"
#include "../includes/webserv.hpp"

#define DEFAULT_CONFIG_PATH "test_files/configs/default.conf"

//TODO: IMPLEMENT PROJECT MAIN
//TEST MAIN
int	main(int ac, char **av)
{
	ServerManager::ServerBlocks	blocks;
	std::string					configPath;

	if (ac == 2)
		configPath = av[1];
	else if (ac == 1)
		configPath = DEFAULT_CONFIG_PATH;
	//else
		

	try
	{
		ConfigParser::parseConfigs(configPath.c_str(), blocks);
	}
	catch (ExceptionMaker const &exception)
	{
		Utils::log(exception.what(), Utils::LOG_ERROR);
		return (1);
	}

	ServerManager server(blocks);
	return (0);
}
