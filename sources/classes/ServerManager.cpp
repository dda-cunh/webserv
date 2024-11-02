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
			event.events = EPOLLIN;
			event.data.fd = it->second->fd();
			if (!doEpollCtl(EPOLL_CTL_ADD, event))
				return (false);
		}
	}
	return (true);
}

void ServerManager::up()	throw()
{
	epoll_event	event;
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
				{
					if (this->isServerSocket(_ep_events[i].data.fd))
					{
						int	client_fd;

						client_fd = accept(_ep_events[i].data.fd, NULL, NULL);
						if (client_fd == -1)
							syscall_kill();
						LOG("New client connected", Utils::LOG_INFO);
						event.events = EPOLLIN | EPOLLET;
						event.data.u64 = ((uint64_t)_ep_events[i].data.fd << 32) | client_fd;
						event.data.fd = client_fd;
						if (!doEpollCtl(EPOLL_CTL_ADD, event))
							syscall_kill();
					}
					else
					{
						Request req(_ep_events[i].data.fd);
						event.events = EPOLLOUT | EPOLLET;
						event.data.u64 = _ep_events[i].data.u64;
						LOG("Request received", Utils::LOG_INFO);
						LOG(req.str(), Utils::LOG_INFO);
						if (!doEpollCtl(EPOLL_CTL_MOD, event))
						{
							LOG(strerror(errno),
										Utils::LOG_WARNING);
							close(event.data.fd);
							continue;
						}
						this->_req_feed.insert(RequestFeed::value_type(_ep_events[i].data.fd, req));
					}
				}
				else if (_ep_events[i].events & EPOLLOUT)
				{
					RequestFeed::iterator	it;

					it = this->_req_feed.find(_ep_events[i].data.fd);
					if (it != this->_req_feed.end())
					{
						int				socket_fd	= (uint32_t)(event.data.u64 >> 32);
						ServerConfig	vServer		= this->_getServerFromSocket(socket_fd);

						Response response(it->second, vServer);
						std::string responseStr = response.getResponse();
						send(_ep_events[i].data.fd, responseStr.c_str(),
								responseStr.length(), MSG_DONTWAIT | MSG_NOSIGNAL);
						if (it->second.header("connection") != "keep-alive")
						{
							doEpollCtl(EPOLL_CTL_DEL, _ep_events[i]);
							close(_ep_events[i].data.fd);
						}
						else
						{
							event.events = EPOLLIN | EPOLLET;
							event.data.fd = _ep_events[i].data.fd;
							if (!doEpollCtl(EPOLL_CTL_MOD, event))
								syscall_kill();
						}
						this->_req_feed.erase(it);
						LOG("Response sent", Utils::LOG_INFO);
					}
					else
					{
						if (_ep_events[i].data.fd != -1)
							close(_ep_events[i].data.fd);
						LOG("Couldn't find request for client",
									Utils::LOG_WARNING);
					}
				}
			}
			catch (ExceptionMaker const &ex)
			{
				LOG(ex.what(), Utils::LOG_WARNING);
			}
		}
	}
	this->down();
}

//use socket_fd to identify the socket inside this->_sockets
ServerConfig const	&ServerManager::_getServerFromSocket(int const& socket_fd)
{
	size_t	i;

	i = 0;

//	std::cout << "FD of Socket that got the request: " << socket_fd << std::endl;
	
	for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
	{
/*
		std::cout << "i: " << i << std::endl;
		std::cout << "socket fd: " << this->_sockets[i].fd() << std::endl;
		std::cout << "Host: " << Network::iPV4PackedTos(this->_sockets[i].address() ) << std::endl;
		std::cout << "Port: " << this->_sockets[i].port() << std::endl;
*/
		if (it->second->fd() == socket_fd )
			return (this->_server_blocks[i] );	//	WILL HAVE TO COUNT ON CHECKING AGAINST server_name
		i++;
	}

	return (this->_server_blocks[0] );	//	OR THROW EXCEPTION IF NO SERVER FOUND?
}

bool ServerManager::isServerSocket(int const &fd) throw()
{
	for (IDSockMap::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); it++)
		if (fd == it->second->fd())
			return (true);
	return (false);
}

bool ServerManager::doEpollCtl(int const &op, epoll_event &ev)	throw()
{
	uint32_t	ev_fd;

	ev_fd = (uint32_t)(ev.data.u64 & 0xFFFFFFFF);
	if (this->_ep_fd == -1)
		return (false);
	if (epoll_ctl(this->_ep_fd, op, ev_fd, &ev) == -1)
		return (false);
	return (true);
}
/**************************************************************************/
