#include "../../includes/classes/Request.hpp"

/****************************  CANNONICAL FORM  ****************************/
Request::Request(void)
	:	_server_config(),
		_client_fd(-1)
{}

Request::Request(Request const & src)
	:	_server_config(),
		_client_fd(-1)
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
		_client_fd(clientFD)
{

	if (this->_client_fd == -1)
		throw (ExceptionMaker("Bad client FD for request"));

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

std::ustring const&	Request::getUri()	const
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
	return ("");
}

void	Request::read()
{
	std::ustring	body;

	//TODO: Read request
	this->parseBody(body);
}

void	Request::parseBody(std::ustring const& unparsed)
{
	//TODO:
}
/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
void	Request::lowerStr(std::string &str)
{
	for (std::string::size_type i = 0; i < str.size(); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
}
/**************************************************************************/
