#include "../../includes/classes/Request.hpp"
#include <cstring>
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
{}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
Request::Request(ServerConfig const& config, Socket const& clientFD)
	:	_server_config(config),
		_client_fd(clientFD),
		_unique_id(genUniqueID(clientFD))
{
	if (this->_client_fd <= STDERR_FILENO)
		throw (ExceptionMaker("Bad client FD for request"));
	this->readClient();
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
void	Request::readClient()
{
	std::ustring	body;
	std::string		line;

	//TODO: Read request
	this->parseBody(body);
}

void	Request::parseBody(std::ustring const& unparsed)
{
	(void) unparsed;
	//TODO:
	// this->_body = unparsed;
}

std::ustring	Request::getNextChunkClient()
{
	unsigned char	buff[CLIENT_CHUNK_SIZE];
	std::ustring	chunk;
	long			r;

	r = read(this->getClientFD(), buff, CLIENT_CHUNK_SIZE);
	if (r <= 0)
		return (std::ustring());
	chunk.assign(buff, buff + r);
	return (chunk);
}

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
/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
std::string const	_expected_version = "HTTP/1.1";

long	Request::genUniqueID(Socket const& client_fd)
{
	return (client_fd + std::clock());
}

void	Request::lowerStr(std::string &str)
{
	for (std::string::size_type i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
}
/**************************************************************************/
