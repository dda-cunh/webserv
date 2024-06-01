#include "../../includes/classes/Server.hpp"

Server::Server(void) : _name("Default")
{
	std::cout << CLI_COLORS_RED << "Server Default constructor called" << CLI_COLORS_RESET << std::endl;
	return ;
}

Server::Server(Server const & src)
{
	std::cout << CLI_COLORS_RED << "Server Copy constructor called" << CLI_COLORS_RESET << std::endl;
	//TODO:
    return ;
}

Server & Server::operator=(Server const & rhs)
{
	std::cout << CLI_COLORS_RED << "Server Copy Assignement operator called"
                << CLI_COLORS_RESET << std::endl;
	if (this != &rhs)
	{
		this->_name = rhs._name;
	}
	return (*this);
}

Server::~Server(void)
{
	std::cout << CLI_COLORS_RED << "Server Destructor called" << CLI_COLORS_RESET
                << std::endl;
	return ;
}
