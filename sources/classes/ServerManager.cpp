#include "../../includes/classes/ServerManager.hpp"
#include <vector>

/****************************  CANNONICAL FORM  ***************************/
ServerManager::ServerManager()
	:	_server_blocks(),
		_is_up(false)
{}

ServerManager::ServerManager(ServerManager const & src)
	:	_server_blocks(),
		_is_up(false)
{
	*this = src;
}

ServerManager & ServerManager::operator=(ServerManager const & rhs)
{
	(void) rhs;
	return (*this);
}

ServerManager::~ServerManager()
{}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
ServerManager::ServerManager(ServerBlocks const& server_blocks)
	:	_server_blocks(server_blocks),
		_is_up(false)
{
	std::vector<int>	unique_endpoints;

	for (ServerBlocks::size_type i = 0; i < server_blocks.size(); i++)
	{
		//TODO: get unique connections
	}
	//TODO: initialize connections
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
void	ServerManager::down()
{
	this->_is_up = false;
}

void	ServerManager::up()
{
	this->_is_up = true;
	while (this->_is_up)
	{
		//TODO: main looping logic, epoll and stuff
	}
}
/**************************************************************************/
