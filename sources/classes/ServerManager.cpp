#include "../../includes/classes/ServerManager.hpp"
#include "../../includes/classes/Response.hpp"
#include "../../includes/classes/Request.hpp"

#define syscall_kill()										\
	{														\
		if (errno != 0)										\
			Utils::log(strerror(errno), Utils::LOG_ERROR);	\
		this->down();										\
		return;												\
	}

/****************************  CANNONICAL FORM  ***************************/
ServerManager::ServerManager()	throw()
	:	_server_blocks(),
		_is_up(false),
		_epoll_fd(-1)
{}

ServerManager::ServerManager(ServerManager const& src)	throw()
	:	_server_blocks(),
		_is_up(false),
		_epoll_fd(-1)
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
}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
ServerManager::ServerManager(ServerBlocks const& server_blocks)	throw()
	:	_server_blocks(server_blocks),
		_is_up(false),
		_epoll_fd(-1)
{
	SocketArr::size_type	unique_i;

	for (ServerBlocks::size_type i = 0; i < server_blocks.size(); i++)
	{
		try
		{
			TCPSocket	socket(server_blocks[i].ipv4, server_blocks[i].port, 5);

			for (unique_i = 0; unique_i < this->_sockets.size(); unique_i++)
				if (this->_sockets[unique_i] == socket)
					break ;
			if (unique_i == this->_sockets.size())
			{
				this->_sockets.push_back(socket);
				this->_sockets.back().connect();
				Utils::log(this->_sockets.back().str() + std::string(" created"), Utils::LOG_INFO);
			}
		}
		catch (ExceptionMaker const &ex)
		{
			Utils::log(ex.what(), Utils::LOG_ERROR);
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
	for (SocketArr::size_type i = 0; i < this->_sockets.size(); i++)
		this->_sockets[i].disconnect();
	if (this->_epoll_fd != -1)
		close(this->_epoll_fd);
	this->_epoll_fd = -1;
	this->_is_up = false;
	Utils::log("ServerManager is down", Utils::LOG_INFO);
}

void ServerManager::up()	throw()
{
	unsigned long const	events_size = ServerManager::_max_events
										* this->_sockets.size();

	epoll_event			events[events_size];
	epoll_event			event;
	int					n_fds;

	if (this->_epoll_fd == -1)
	{
		this->_epoll_fd = epoll_create(events_size);
		if (this->_epoll_fd == -1)
			syscall_kill();
		for (SocketArr::size_type i = 0; i < this->_sockets.size(); i++)
		{
			event.events = EPOLLIN;
			event.data.fd = this->_sockets[i].fd();
			if (!doEpollCtl(EPOLL_CTL_ADD, event))
				syscall_kill();
		}
	}
	this->_is_up = true;
	Utils::log("ServerManager is up", Utils::LOG_INFO);
	while (this->_is_up)
	{
		n_fds = epoll_wait(this->_epoll_fd, events, events_size, -1);
		if (n_fds == -1)
			syscall_kill();
		for (int i = 0; i < n_fds; i++)
		{
			try
			{
				if (events[i].events & EPOLLIN)
				{
					if (this->isServerSocket(events[i].data.fd))
					{
						int	client_fd;

						client_fd = accept(events[i].data.fd, NULL, NULL);
						if (client_fd == -1)
							syscall_kill();
						Utils::log("New client connected", Utils::LOG_INFO);
						event.events = EPOLLIN | EPOLLET;
						event.data.fd = client_fd;
						if (!doEpollCtl(EPOLL_CTL_ADD, event))
							syscall_kill();
					}
					else
					{
						Request req(events[i].data.fd);
						event.events = EPOLLOUT | EPOLLET;
						event.data.fd = events[i].data.fd;
						Utils::log("Request received", Utils::LOG_INFO);
						Utils::log(req.str(), Utils::LOG_INFO);
						if (!doEpollCtl(EPOLL_CTL_MOD, event))
						{
							Utils::log(strerror(errno),
										Utils::LOG_WARNING);
							close(event.data.fd);
							continue;
						}
						this->_req_feed.insert(RequestFeed::value_type(events[i].data.fd, req));
					}
				}
				else if (events[i].events & EPOLLOUT)
				{
					RequestFeed::iterator	it;
					it = this->_req_feed.find(events[i].data.fd);
					if (it != this->_req_feed.end())
					{
						Response response(it->second);
						std::string responseStr = response.response();
						send(events[i].data.fd, responseStr.c_str(), responseStr.length(), 0);
						if (it->second.header("connection") != "keep-alive")
							close(events[i].data.fd);
						else
						{
							event.events = EPOLLIN | EPOLLET;
							event.data.fd = events[i].data.fd;
							if (!doEpollCtl(EPOLL_CTL_MOD, event))
								syscall_kill();
						}
						this->_req_feed.erase(it);
						Utils::log("Response sent", Utils::LOG_INFO);
					}
					else
					{
						if (events[i].data.fd != -1)
							close(events[i].data.fd);
						Utils::log("Couldn't find request for client",
									Utils::LOG_WARNING);
					}
				}
			}
			catch (ExceptionMaker const &ex)
			{
				Utils::log(ex.what(), Utils::LOG_WARNING);
			}
		}
	}
	this->down();
}

bool ServerManager::isServerSocket(int const &fd) throw()
{
	for (SocketArr::size_type j = 0; j < this->_sockets.size(); j++)
		if (fd == this->_sockets[j].fd())
			return (true);
	return (false);
}

bool ServerManager::doEpollCtl(int const &op, epoll_event &ev)	throw()
{
	if (this->_epoll_fd == -1)
		return (false);
	if (epoll_ctl(this->_epoll_fd, op, ev.data.fd, &ev) == -1)
		return (false);
	return (true);
}
/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
unsigned long const ServerManager::_max_events = 93;
/**************************************************************************/
