#include <cerrno>
#include <cstring>
#include <limits>

#include "../../includes/classes/TCPSocket.hpp"
#include "../../includes/webserv.hpp"

/****************************  CANNONICAL FORM  ****************************/
TCPSocket::TCPSocket(void)	throw()
:	_address(),
	_port(),
	_backlog(),
	_fd(-1)
{}

TCPSocket::TCPSocket(TCPSocket const & src)	throw()
:	_address(src._address),
	_port(src._port),
	_backlog(src._backlog)
{
	*this = src;
}

TCPSocket & TCPSocket::operator=(TCPSocket const & rhs)	throw()
{
	this->_fd = rhs.fd();
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
std::string	TCPSocket::address()	const	throw()
{
	return (Network::iPV4PackedTos(this->_address));
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
		close (this->_fd);
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
/**************************************************************************/

/*****************************  OP OVERLOADS  *****************************/
bool	TCPSocket::operator==(TCPSocket const & rhs)	const	throw()
{
	return (this->_address == rhs._address && this->_port == rhs._port);
}
/**************************************************************************/

/*****************************  NON MEMBERS  ******************************/

std::ostream &	operator<<(std::ostream & o, TCPSocket const& i)	throw()
{
	o << "TCPSocket: " << i.fd() << " (" << i.address();
	o << ":" << i.port() << ")";
	return (o);
}
/**************************************************************************/
