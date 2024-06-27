#include <limits>

#include "../../includes/classes/TCPSocket.hpp"
#include "../../includes/webserv.hpp"

/****************************  CANNONICAL FORM  ****************************/
TCPSocket::TCPSocket(void)
:	_address(),
	_port(),
	_backlog(),
	_fd(-1)
{}

TCPSocket::TCPSocket(TCPSocket const & src)
:	_address(src._address),
	_port(src._port),
	_backlog(src._backlog),
	_fd(-1)
{
	*this = src;
}

TCPSocket & TCPSocket::operator=(TCPSocket const & rhs)
{
	(void) rhs;
	return (*this);
}

TCPSocket::~TCPSocket(void)
{
	if (this->_fd != -1)
		close (this->_fd);
}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
TCPSocket::TCPSocket(uint64_t const& packed, uint32_t const& port,
							int const& backlog)	throw()
:	_address(packed),
	_port(port),
	_fd(-1),
	_backlog(backlog)
{}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
int const&	TCPSocket::fd()	const
{
	return (this->_fd);
}

void	TCPSocket::connect()
{
	struct sockaddr_in	address;
	int					addrlen;

	if (this->_address > std::numeric_limits<uint32_t>::max())
		throw(ExceptionMaker("Bad adress for socket"));
	if (this->_port > std::numeric_limits<uint16_t>::max())
		throw(ExceptionMaker("Bad adress for socket"));
	if (this->_fd != -1)
		close (this->_fd);
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_fd == -1)
		throw(ExceptionMaker("Socket creation failed"));
	addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(this->_address);
	address.sin_port = htons(this->_port);
	if (bind(this->_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		close(this->_fd);
		this->_fd = -1;
		throw(ExceptionMaker("bind() failed on TCPSocket"));
	}
	if (listen(this->_fd, this->_backlog) == -1)
	{
		close(this->_fd);
		this->_fd = -1;
		throw(ExceptionMaker("listen() failed on TCPSocket"));
	}
}
/**************************************************************************/

/*****************************  OP OVERLOADS  *****************************/
bool	TCPSocket::operator=(TCPSocket const & rhs) const
{
	return (this->_address == rhs._address && this->_port == rhs._port);
}
/**************************************************************************/

/*****************************  NON MEMBERS  ******************************/

std::ostream &	operator<<(std::ostream &, TCPSocket const&);
/**************************************************************************/
