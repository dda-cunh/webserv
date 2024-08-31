#include "../includes/classes/ServerManager.hpp"
#include "../includes/webserv.hpp"


//TODO: IMPLEMENT PROJECT MAIN
//TEST MAIN
int	main(int ac, char **av)
{
	(void) ac;
	(void) av;

	ServerManager::ServerBlocks	blocks;
	ServerConfig				config;

	blocks.push_back(config);
	ServerManager server(blocks);
	return (0);
}
