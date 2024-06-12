#include "../../includes/classes/Server.hpp"

/****************************  CANNONICAL FORM  ****************************/
Server::Server(void) : _name("Default")
{}

Server::Server(Server const & src)
{
	*this = src;
}

Server & Server::operator=(Server const & rhs)
{
	this->_name = rhs._name;
	return (*this);
}

Server::~Server(void)
{}
/**************************************************************************/
