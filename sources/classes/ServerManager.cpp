#include "../../includes/classes/ServerManager.hpp"
#include "../../includes/classes/Response.hpp"
#include "../../includes/classes/Request.hpp"

#define syscall_kill()										\
	{														\
		if (errno != 0)										\
			LOG(strerror(errno), Utils::LOG_ERROR);	\
		this->down();										\
		return;												\
	}



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
				LOG(sock->str() + std::string(" created"),
					Utils::LOG_INFO);
			}
			
		}
		catch (ExceptionMaker const &ex)
		{
			LOG(ex.what(), Utils::LOG_ERROR);
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
	for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		it->second->disconnect();
	if (this->_ep_fd != -1)
	{
		for (unsigned int i = 0; i < SM_EP_EV_LEN; i++)
			doEpollCtl(EPOLL_CTL_DEL, _ep_events[i]);
		close(this->_ep_fd);
	}
	this->_ep_fd = -1;
	this->_is_up = false;
	LOG("ServerManager is down", Utils::LOG_INFO);
}

bool	ServerManager::initEpoll()	throw()
{
	if (this->_ep_fd == -1)
	{
		epoll_event	event;

		std::memset(this->_ep_events, 0, SM_EP_EV_LEN * sizeof(epoll_event));
		this->_ep_fd = epoll_create(SM_EP_EV_LEN);
		if (this->_ep_fd == -1)
			return (false);
		for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		{
			EpollData * data = new EpollData;
			*data = (EpollData){std::string(), 0, 0, it->second->fd()};
			event.events = EPOLLIN;
			event.data.u64 = EpollDatatoU64(data);
			if (!doEpollCtl(EPOLL_CTL_ADD, event))
				return (false);
		}
	}
	return (true);
}

void ServerManager::up()	throw()
{
	int			n_fds;

	if (!initEpoll())
		syscall_kill();
	this->_is_up = true;
	LOG("ServerManager is up", Utils::LOG_INFO);
	while (this->_is_up)
	{
		n_fds = epoll_wait(this->_ep_fd, _ep_events, SM_EP_EV_LEN, -1);
		if (n_fds == -1)
			syscall_kill();
		for (int i = 0; i < n_fds; i++)
		{
			try
			{
				if (_ep_events[i].events & EPOLLIN)
					readEvent(_ep_events[i]);
				else if (_ep_events[i].events & EPOLLOUT)
					writeEvent(_ep_events[i]);
			}
			catch (ExceptionMaker const &ex)
			{
				LOG(ex.what(), Utils::LOG_WARNING);
			}
		}
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
//		std::cout << serverName << std::endl;

		for (size_t i = 0; i < matchedConfigs.size(); i++)
		{
			for (size_t j = 0; j < matchedConfigs.at(i).getServerNamesSize(); j++)
			{
				if (matchedConfigs.at(i).getServerName(j) == serverName)
				{
					std::cout << "MATCHED SERVER:" << std::endl;
					std::cout << matchedConfigs.at(i) << std::endl;
					return (matchedConfigs.at(i) );
				}
			}
		}
		return (matchedConfigs.at(0) );
	}
	else
		throw (ExceptionMaker("Could not find socket") );
//	return (_server_blocks[0]);
}

bool ServerManager::doEpollCtl(int const &op, epoll_event &ev)	throw()
{
	EpollData *	data;

	data = u64toEpollData(ev.data.u64);
	if (!data)
		return (true);
	if (data->ownFD == -1 || epoll_ctl(this->_ep_fd, op, data->ownFD, &ev) == -1)
	{
		delete data;
		return (false);
	}
	if (op == EPOLL_CTL_DEL)
		delete data;
	return (true);
}

void	ServerManager::readEvent(epoll_event & trigEv)	throw()
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
		LOG("New client connected", Utils::LOG_INFO);
		reqData = new EpollData;
		*reqData = (EpollData){std::string(), 0, trigData->ownFD, client_fd};
		reqEv.events = EPOLLIN;
		reqEv.data.u64 = EpollDatatoU64(reqData);
		if (!doEpollCtl(EPOLL_CTL_ADD, reqEv))
			syscall_kill();
	}
	else
	{
		Request			req(trigData->ownFD);
		ServerConfig	vServer = getServerFromSocket(trigData->parentFD, req);

		trigData->responseStr = Response(req, vServer).getResponse();
		trigData->keepAlive = req.header("connection") == "keep-alive";
		trigEv.events = EPOLLOUT;
		LOG("Request received", Utils::LOG_INFO);
		LOG(req.str(), Utils::LOG_INFO);
		if (!doEpollCtl(EPOLL_CTL_MOD, trigEv))
		{
			LOG(strerror(errno), Utils::LOG_WARNING);
			close(trigData->ownFD);
			return ;
		}
	}
}

void ServerManager::writeEvent(epoll_event & trigEv) throw()
{
	EpollData *	trigData;
	ssize_t		bytesSent;

	trigData = u64toEpollData(trigEv.data.u64);
	if (!trigData)
		return ;
	bytesSent = send(trigData->ownFD, trigData->responseStr.c_str(),
						trigData->responseStr.length(), MSG_DONTWAIT | MSG_NOSIGNAL);
	if (bytesSent > 0)
		trigData->responseStr = trigData->responseStr.substr(bytesSent);
	if (!trigData->responseStr.empty())
		return ;
	if (!trigData->keepAlive)
	{
		doEpollCtl(EPOLL_CTL_DEL, trigEv);
		close(trigData->ownFD);
	}
	else
	{
		trigEv.events = EPOLLIN;
		if (!doEpollCtl(EPOLL_CTL_MOD, trigEv))
			syscall_kill();
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
