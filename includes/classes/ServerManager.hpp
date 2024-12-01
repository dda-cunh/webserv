#pragma once

#include "../webserv.hpp"
#include "TCPSocket.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
//#include "ServerLocation.hpp"

#include <sys/epoll.h>

#define SM_EP_EV_LEN 1024

class ServerConfig;

typedef struct s_epoll_data
{
	std::string	responseStr;
	ByteArr		reqBytes;
	bool		keepAlive;
	int			parentFD;
	int			ownFD;
}	EpollData;

class ServerManager
{
	public:
		typedef std::map<uint64_t, TCPSocket*>	IDSockMap;
		typedef std::map<int, ServerConfig>		SockFDConfMap;
		typedef std::vector<ServerConfig>		ServerBlocks;

		~ServerManager()										throw();

		ServerManager(ServerBlocks const& blocks)				throw();

		bool	initEpoll()										throw();
		void	down()											throw();
		void	up()											throw();
	private:
		ServerBlocks const	_server_blocks;

		SockFDConfMap		_sockFD_confI;
		epoll_event			_ep_events[SM_EP_EV_LEN];
		IDSockMap			_sockets;
		bool				_is_up;
		int					_ep_fd;

		ServerManager & operator=(ServerManager const & rhs)	throw();
		ServerManager(ServerManager const & src)				throw();
		ServerManager()											throw();

		bool				doEpollCtl(int const& op,
										epoll_event & ev)		throw();
		EpollData *			u64toEpollData(uint64_t const& l)	throw();
		uint64_t			EpollDatatoU64(EpollData * d)		throw();
		void				writeEvent(epoll_event & trigEv);
		void				readEvent(epoll_event & trigEv);
		ServerConfig const	getServerFromSocket(int const& socket_fd, Request const &request);
};
