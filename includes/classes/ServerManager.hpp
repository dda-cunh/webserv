#pragma once

#include "../webserv.hpp"
#include "TCPSocket.hpp"
#include "ServerConfig.hpp"
//#include "ServerLocation.hpp"

#include <sys/epoll.h>

#define SM_EP_EV_LEN 1024

class ServerConfig;

class ServerManager
{
	public:
		typedef std::vector<ServerConfig>	ServerBlocks;
		typedef std::map<int, Request>		RequestFeed;
		typedef std::vector<TCPSocket>		SocketArr;

		~ServerManager()										throw();

		ServerManager(ServerBlocks const& blocks)				throw();

		bool	initEpoll()										throw();
		void	down()											throw();
		void	up()											throw();
	private:
		ServerBlocks const			_server_blocks;

		epoll_event					_ep_events[SM_EP_EV_LEN];
		RequestFeed					_req_feed;
		SocketArr					_sockets;
		bool						_is_up;
		int							_ep_fd;

		ServerManager & operator=(ServerManager const & rhs)	throw();
		ServerManager(ServerManager const & src)				throw();
		ServerManager()											throw();

		bool				doEpollCtl(int const& op, epoll_event & ev)		throw();
		bool				isServerSocket(int const& fd)					throw();
		ServerConfig const	&_getServerFromSocket(uint32_t socket_fd);
};
