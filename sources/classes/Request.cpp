#include "../../includes/classes/Request.hpp"
#include <ctime>

/****************************  CANNONICAL FORM  ****************************/
Request::Request(void)
	:	_server_config(),
		_client_fd(-1),
		_unique_id()
{}

Request::Request(Request const & src)
	:	_server_config(),
		_client_fd(-1),
		_unique_id()
{
	*this = src;
}

Request & Request::operator=(Request const & rhs)
{
	(void) rhs;
	return (*this);
}

Request::~Request(void)
{
	LongStrMap::iterator	it;

	it = Request::_client_buffers.find(this->_unique_id);
	if (it != Request::_client_buffers.end())
		Request::_client_buffers.erase(it);
}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
Request::Request(ServerConfig const& config, Socket const& clientFD)
	:	_server_config(config),
		_client_fd(clientFD),
		_unique_id(genUniqueID(clientFD))
{
	if (this->_client_fd == -1)
		throw (ExceptionMaker("Bad client FD for request"));
	this->read();
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
ServerConfig const&	Request::getServerConfig()	const
{
	return (this->_server_config);
}

HTTP_VERSION const&	Request::getVersion()	const
{
	return (this->_version);
}

std::ustring const&	Request::getBody()	const
{
	return (this->_body);
}

std::string const&	Request::getUri()	const
{
	return (this->_uri);
}

HTTP_METHOD const&	Request::getMethod()	const
{
	return (this->_method);
}

Socket const&	Request::getClientFD()	const
{
	return (this->_client_fd);
}

std::string const	Request::getHeader(std::string header)
{
	StrStrMap::iterator	it;

	Request::lowerStr(header);
	it = this->_headers.find(header);
	if (it != this->_headers.end())
		return (it->second);
	return (NO_SUCH_HEADER);
}

void	Request::read()
{
	std::ustring	body;
	std::string		line;

	//TODO: Read request
	this->parseBody(body);
}

void	Request::parseBody(std::ustring const& unparsed)
{
	//TODO:
}
/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
std::string const	_expected_version = "HTTP/1.1";

std::string	Request::gNLClient()
{
	std::string &	buffer = Request::_client_buffers[this->_unique_id];

}

long	genUniqueID(Socket const& client_fd)
{
	return (client_fd + std::clock());
}

void	Request::lowerStr(std::string &str)
{
	for (std::string::size_type i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
}

Request::LongStrMap	Request::_client_buffers = LongStrMap();

/**************************************************************************/
