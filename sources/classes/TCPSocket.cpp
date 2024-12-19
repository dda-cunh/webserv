#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <sstream>
#include <cstring>

#include "../../includes/namespaces/Network.hpp"

#include "../../includes/classes/ExceptionMaker.hpp"
#include "../../includes/classes/TCPSocket.hpp"

/****************************  CANNONICAL FORM  ****************************/
TCPSocket::TCPSocket(void)	throw()
:	_address(0),
	_port(0),
	_backlog(0),
	_fd(-1)
{}

TCPSocket::TCPSocket(TCPSocket const & src)	throw()
:	_address(src._address),
	_port(src._port),
	_backlog(src._backlog),
	_fd(-1)
{
	*this = src;
}

TCPSocket & TCPSocket::operator=(TCPSocket const & rhs)	throw()
{
	if (rhs._fd != -1)
		close(rhs.fd());
	this->connect();
	return (*this);
}

TCPSocket::~TCPSocket(void)	throw()
{
	this->disconnect();
}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
TCPSocket::TCPSocket(uint32_t const& packed, uint16_t const& port,
							int const& backlog)	throw()
:	_address(packed),
	_port(port),
	_backlog(backlog),
	_fd(-1)
{}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
uint32_t	TCPSocket::address()	const	throw()
{
	return (this->_address);
}

std::string	TCPSocket::str()	const	throw()
{
	std::stringstream ss;

	ss << *this;
	return (ss.str());
}

uint16_t	TCPSocket::port()	const	throw()
{
	return (this->_port);
}

int	TCPSocket::fd()	const	throw()
{
	return (this->_fd);
}

void	TCPSocket::disconnect()	throw()
{
	if (this->_fd != -1)
		close(this->_fd);
	this->_fd = -1;
}

void	TCPSocket::connect()
{
	struct sockaddr_in	address;
	int					val;    

	if (this->_fd != -1)
		return ;
	if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		this->badSyscallThrow();
	val = 1;
	if ((setsockopt(this->fd(), SOL_SOCKET, SO_REUSEADDR, &val,
			sizeof(val))) == -1)
		this->badSyscallThrow();	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(this->_address);
	address.sin_port = htons(this->_port);
	if (bind(this->_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		this->badSyscallThrow();
	if (listen(this->_fd, this->_backlog) == -1)
		this->badSyscallThrow();
}

void	TCPSocket::badSyscallThrow()
{
	if (this->fd() != -1)
	{
		close(this->_fd);
		this->_fd = -1;
	}
	throw(ExceptionMaker(strerror(errno)));
}

uint64_t	TCPSocket::socketToPacked(void)	const	throw()
{
	return (TCPSocket::socketToPacked(this->address(), this->port()));
}
/**************************************************************************/

/*****************************  OP OVERLOADS  *****************************/
bool	TCPSocket::operator==(TCPSocket const & rhs)	const	throw()
{
	return (socketToPacked(this->_address, this->port())
			== socketToPacked(rhs._address, rhs.port()));
}
/**************************************************************************/

/****************************  STATIC MEMBERS  ****************************/
uint64_t			TCPSocket::socketToPacked(uint32_t const& ip, uint16_t const& port)	throw()
{
	return ((uint64_t) ip << (sizeof(uint16_t) * 8) | port);
}
/**************************************************************************/

/*****************************  NON MEMBERS  ******************************/

std::ostream &	operator<<(std::ostream & o, TCPSocket const& i)	throw()
{
	o << "TCPSocket: " << i.fd() << " (" << Network::iPV4PackedTos(i.address());
	o << ":" << i.port() << ")";
	return (o);
}
/**************************************************************************/
