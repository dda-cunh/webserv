#include "../includes/classes/ServerManager.hpp"
#include "../includes/webserv.hpp"


//TODO: IMPLEMENT PROJECT MAIN
//TEST MAIN
int	main(int ac, char **av)
{
	(void) ac;
	(void) av;

	ServerManager::ServerBlocks	blocks;
	blocks.push_back((ServerConfig){Network::sToIPV4Packed("127.0.0.1"), 5050});
	ServerManager server(blocks);
	return (0);
}
