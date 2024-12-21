#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <cstring>

#include "../../includes/classes/ExceptionMaker.hpp"
#include "../../includes/classes/ServerManager.hpp"
#include "../../includes/classes/Response.hpp"
#include "../../includes/classes/Request.hpp"
#include "../../includes/classes/LogFeed.hpp"

namespace SigIntHandler
{
    bool    g_sigIntSet;

	void handler(int sig)
    {
        if (sig == SIGINT)
            g_sigIntSet = !g_sigIntSet;
    }
};

#define syscall_kill()										\
	{														\
		if (errno != 0)										\
			LOGFEED.buff(strerror(errno), Utils::LOG_ERROR);\
		this->down();										\
		return;												\
	}

#define EPOLL_CLOSED_FLAGS	EPOLLRDHUP | EPOLLHUP

/****************************  CANNONICAL FORM  ***************************/
ServerManager::ServerManager()	throw()
	:	_server_blocks(),
		_is_up(false),
		_ep_fd(-1)
{}

ServerManager::ServerManager(ServerManager const& src)	throw()
	:	_server_blocks(),
		_is_up(false),
		_ep_fd(-1)
{
	*this = src;
}

ServerManager &ServerManager::operator=(ServerManager const &rhs)	throw()
{
	(void) rhs;
	return (*this);
}

ServerManager::~ServerManager()	throw()
{
	this->down();
	for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		delete it->second;
}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
ServerManager::ServerManager(ServerBlocks const& server_blocks)	throw()
	:	_server_blocks(server_blocks),
		_is_up(false),
		_ep_fd(-1)
{
	uint64_t	id;

	for (ServerBlocks::size_type i = 0; i < server_blocks.size(); i++)
	{
		try
		{
			id = TCPSocket::socketToPacked(server_blocks[i].getHost(),
											server_blocks[i].getPort());
			if (this->_sockets.find(id) == this->_sockets.end())
			{
				TCPSocket *	sock;

				sock = new TCPSocket(server_blocks[i].getHost(),
										server_blocks[i].getPort(), 5);
				this->_sockets.insert(IDSockMap::value_type(id, sock));
				sock->connect();
				_sockFD_confI.insert(SockFDConfMap::value_type(sock->fd(), _server_blocks[i]));
				LOGFEED.buff(sock->str() + std::string(" created"),
					Utils::LOG_INFO);
			}
			
		}
		catch (ExceptionMaker const &ex)
		{
			LOGFEED.buff(ex.what(), Utils::LOG_ERROR);
			return ;
		}

	}
	if (this->_sockets.empty())
		return ;
	this->up();
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
void ServerManager::down()	throw()
{
	epoll_event	ev;

	if (this->_ep_fd != -1)
	{
		EpollDataMap::iterator	it;

		while (!_ep_data_map.empty())
		{
			it = _ep_data_map.begin();
			ev.events = EPOLLIN;
			ev.data.u64 = EpollDatatoU64(it->second);
			doEpollCtl(EPOLL_CTL_DEL, ev);
		}
		close(this->_ep_fd);
		this->_ep_fd = -1;
	}
	for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		it->second->disconnect();
	this->_is_up = false;
	LOGFEED.buff("ServerManager is down", Utils::LOG_INFO);
}

bool	ServerManager::initEpoll()	throw()
{
	if (this->_ep_fd == -1)
	{
		epoll_event	event;
		EpollData *	data;

		std::memset(this->_ep_events, 0, SM_EP_EV_LEN * sizeof(epoll_event));
		this->_ep_fd = epoll_create(SM_EP_EV_LEN);
		if (this->_ep_fd == -1)
			return (false);
		for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		{
			data = new EpollData;
			*data = (EpollData){std::string(), NULL, 0, 0, it->second->fd()};
			event.events = EPOLLIN;
			event.data.u64 = EpollDatatoU64(data);
			if (!doEpollCtl(EPOLL_CTL_ADD, event))
				return (false);
		}
		data = new EpollData;
		*data = (EpollData){std::string(), NULL, 0, 0, STDOUT_FILENO};
		event.events = EPOLLOUT;
		event.data.u64 = EpollDatatoU64(data);
		if (!doEpollCtl(EPOLL_CTL_ADD, event))
			return (false);
	}
	return (true);
}

void ServerManager::up()	throw()
{
	int			n_fds;

	if (!initEpoll())
		syscall_kill();
	this->_is_up = true;
	if (signal(SIGINT, SigIntHandler::handler) == SIG_ERR)
		syscall_kill();
	LOGFEED.buff("ServerManager is up", Utils::LOG_INFO);
	while (this->_is_up)
	{
		n_fds = epoll_wait(this->_ep_fd, _ep_events, SM_EP_EV_LEN, -1);
		if (n_fds == -1)
			syscall_kill();
		for (int i = 0; i < n_fds; i++)
		{
			try
			{
				if (_ep_events[i].events & (EPOLL_CLOSED_FLAGS))
				{
					doEpollCtl(EPOLL_CTL_DEL, _ep_events[i]);
					continue ;
				}
				if (_ep_events[i].events & EPOLLIN)
					readEvent(_ep_events[i]);
				if (_ep_events[i].events & EPOLLOUT)
					writeEvent(_ep_events[i]);
			}
			catch (ExceptionMaker const &ex)
			{
				LOGFEED.buff(ex.what(), Utils::LOG_WARNING);
			}
		}
		std::memset(this->_ep_events, 0, SM_EP_EV_LEN * sizeof(epoll_event));
		this->_is_up = !SigIntHandler::g_sigIntSet;
	}
	this->down();
}

ServerConfig const	ServerManager::getServerFromSocket(int const& socket_fd, Request const &request)
{
	//	COPY ALL ip:port MATCHES TO A VECTOR
	//	AND THEN CHECK IF ANY HAS A servername THAT MATCHES host IN request
	//	IF NONE MATCH, SELECT FIRST FROM LIST
	ServerBlocks	matchedConfigs;
	std::string		serverName = request.header("host").substr(0, request.header("host").find(":") );
	SockFDConfMap::const_iterator	it;

	it = this->_sockFD_confI.find(socket_fd);
	if (it != this->_sockFD_confI.end())	//	WHAT TO DO IF SOCKET NOT FOUND?
	{
		uint32_t		host = it->second.getHost();
		uint16_t		port = it->second.getPort();

		for (size_t i = 0; i < this->_server_blocks.size(); i++)
		{
			if (this->_server_blocks.at(i).getHost() == host && this->_server_blocks.at(i).getPort() == port)
				matchedConfigs.push_back(this->_server_blocks.at(i) );
		}
		for (size_t i = 0; i < matchedConfigs.size(); i++)
		{
			for (size_t j = 0; j < matchedConfigs.at(i).getServerNamesSize(); j++)
			{
				if (matchedConfigs.at(i).getServerName(j) == serverName)
				{
					// std::cout << "MATCHED SERVER:" << std::endl;
					// std::cout << matchedConfigs.at(i) << std::endl; // make .str method to use with LogFeed
					return (matchedConfigs.at(i) );
				}
			}
		}
		return (matchedConfigs.at(0) );
	}
	else
		throw (ExceptionMaker("Could not find socket") );
}

bool ServerManager::doEpollCtl(int const &op, epoll_event &ev)	throw()
{
	EpollData *	data;
	int			fd;

	data = u64toEpollData(ev.data.u64);
	if (!data)
		return (true);
	fd = data->ownFD;
	if (fd == -1)
		return (false);
	if (epoll_ctl(this->_ep_fd, op, fd, &ev) == -1)
		return (false);
	if (op == EPOLL_CTL_DEL)
	{
		_ep_data_map.erase(fd);
		if (data->req)
			delete data->req;
		delete data;
		data = NULL;
		close(fd);
	}
	else if (op == EPOLL_CTL_ADD)
		_ep_data_map[fd] = data;
	return (true);
}

void	ServerManager::readEvent(epoll_event & trigEv)
{
	EpollData *	trigData;

	trigData = u64toEpollData(trigEv.data.u64);
	if (!trigData)
		return ;
	if (trigData->parentFD == 0)
	{
		EpollData *	reqData;
		epoll_event	reqEv;
		int			client_fd;

		client_fd = accept(trigData->ownFD, NULL, NULL);
		if (client_fd == -1)
			syscall_kill();
		LOGFEED.buff("New client connected", Utils::LOG_INFO);
		reqData = new EpollData;
		*reqData = (EpollData){std::string(), NULL, 0, trigData->ownFD, client_fd};
		reqEv.events = EPOLLIN | EPOLL_CLOSED_FLAGS;
		reqEv.data.u64 = EpollDatatoU64(reqData);
		if (!doEpollCtl(EPOLL_CTL_ADD, reqEv))
			syscall_kill();
	}
	else
	{
		ssize_t bytesRead;
		char	buff[CHUNK_SIZE];

		bytesRead = read(trigData->ownFD, buff, CHUNK_SIZE);
		if (bytesRead == 0)
			doEpollCtl(EPOLL_CTL_DEL, trigEv);
		else if (bytesRead > 0)
		{
			if (!trigData->req)
				trigData->req = new Request;
			trigData->req->doParse(std::string(buff, bytesRead));
			if (trigData->req->parsingStage() != REQ_PARSED_BODY)
				return ;
			LOGFEED.buff("Request received", Utils::LOG_INFO);
			LOGFEED.buff(trigData->req->str(), Utils::LOG_INFO);
			ServerConfig	vServer = getServerFromSocket(trigData->parentFD, *trigData->req);
			trigData->responseStr = Response(*trigData->req, vServer).getResponse();
			trigData->keepAlive = trigData->req->header("connection") == "keep-alive";
			delete trigData->req;
			trigData->req = NULL;
			trigEv.events = EPOLLOUT | EPOLL_CLOSED_FLAGS;
			if (!doEpollCtl(EPOLL_CTL_MOD, trigEv))
			{
				LOGFEED.buff(strerror(errno), Utils::LOG_WARNING);
				doEpollCtl(EPOLL_CTL_DEL, trigEv);
			}
		}
		else
		{
			LOGFEED.buff(strerror(errno), Utils::LOG_WARNING);
			doEpollCtl(EPOLL_CTL_DEL, trigEv);
		}
	}
}

void ServerManager::writeEvent(epoll_event & trigEv)
{
	EpollData *	trigData;
	ssize_t		bytesSent;

	trigData = u64toEpollData(trigEv.data.u64);
	if (!trigData)
		return ;
	if (trigData->ownFD < 0)
		return ;
	if (trigData->ownFD == STDOUT_FILENO)
	{
		LOGFEED.log();
		return ;
	}
	bytesSent = write(trigData->ownFD, trigData->responseStr.c_str(), 
						trigData->responseStr.length());
	if (bytesSent >= 0)
	{
		trigData->responseStr = trigData->responseStr.substr(bytesSent);
		if (!trigData->responseStr.empty())
			return ;
		if (!trigData->keepAlive)
			doEpollCtl(EPOLL_CTL_DEL, trigEv);
		else
		{
			trigEv.events = EPOLLIN | EPOLL_CLOSED_FLAGS;
			if (!doEpollCtl(EPOLL_CTL_MOD, trigEv))
				syscall_kill();
		}
	}
	else if (bytesSent == -1)
	{
		LOGFEED.buff(strerror(errno), Utils::LOG_WARNING);
		doEpollCtl(EPOLL_CTL_DEL, trigEv);
	}
}

EpollData *	ServerManager::u64toEpollData(uint64_t const& l)	throw()
{
	return (reinterpret_cast<EpollData*>(l));
}

uint64_t	ServerManager::EpollDatatoU64(EpollData * data)	throw()
{
	return (reinterpret_cast<uint64_t>(data));
}
/**************************************************************************/
