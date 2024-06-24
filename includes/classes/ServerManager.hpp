#pragma once

#include "../webserv.hpp"

class ServerManager
{
	public:
		ServerManager(void);
		ServerManager(ServerManager const & src);
		ServerManager & operator=(ServerManager const & rhs);
		~ServerManager(void);

	private:
        std::string _name;
};
