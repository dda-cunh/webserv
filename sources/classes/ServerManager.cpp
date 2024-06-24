#include "../../includes/classes/ServerManager.hpp"

/****************************  CANNONICAL FORM  ***************************/
ServerManager::ServerManager(void) : _name("Default")
{}

ServerManager::ServerManager(ServerManager const & src)
{
	*this = src;
}

ServerManager & ServerManager::operator=(ServerManager const & rhs)
{
	this->_name = rhs._name;
	return (*this);
}

ServerManager::~ServerManager(void)
{}
/**************************************************************************/
