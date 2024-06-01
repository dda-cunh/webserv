#pragma once

#include "../webserv.hpp"

class Server
{
	public:
		Server(void);
		Server(Server const & src);
		Server & operator=(Server const & rhs);
		virtual ~Server(void);

	private:
        std::string _name;
};
