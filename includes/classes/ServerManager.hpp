#pragma once

#include "../webserv.hpp"
#include "TCPSocket.hpp"

class ServerManager
{
	public:
		typedef std::vector<ServerConfig>	ServerBlocks;
		typedef std::map<int, Request>		RequestFeed;
		typedef std::vector<TCPSocket>		SocketArr;

		~ServerManager()										throw();

		ServerManager(ServerBlocks const& blocks)				throw();

		void	down()											throw();
		void	up()											throw();

	private:
		ServerBlocks const			_server_blocks;

		RequestFeed					_req_feed;
		SocketArr					_sockets;
		bool						_is_up;
		int							_epoll_fd;

		ServerManager & operator=(ServerManager const & rhs)	throw();
		ServerManager(ServerManager const & src)				throw();
		ServerManager()											throw();

		bool	doEpollCtl(int const& op, epoll_event & ev)		throw();
		bool	isServerSocket(int const& fd)					throw();

		static unsigned long const	_max_events;
};
