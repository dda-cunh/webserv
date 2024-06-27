#pragma once

#include "../webserv.hpp"

class ServerManager
{
	public:
		~ServerManager();

		ServerManager(ServerBlocks const&);

		void	down();
		void	up();

	private:
		ServerBlocks const	_server_blocks;

		bool				_is_up;

		ServerManager();
		ServerManager(ServerManager const & src);
		ServerManager & operator=(ServerManager const & rhs);
};
