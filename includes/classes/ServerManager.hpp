#pragma once

#include "../webserv.hpp"
#include "TCPSocket.hpp"

class ServerManager
{
	public:
		typedef std::vector<ServerConfig>	ServerBlocks;
		typedef std::map<int, Request>		RequestFeed;
		typedef std::vector<TCPSocket>		SocketArr;

		~ServerManager();

		ServerManager(ServerBlocks const& blocks);

		void	down();
		void	up();

	private:
		ServerBlocks const	_server_blocks;

		RequestFeed			_req_feed;
		SocketArr			_sockets;
		bool				_is_up;

		ServerManager();
		ServerManager(ServerManager const & src);
		ServerManager & operator=(ServerManager const & rhs);

		bool	isServerSocket(int const& fd)	throw();

		static unsigned long const	_max_events;

		static bool	doEpollCtl(int const& epoll_fd, int const& op,
								epoll_event & ev);
};
