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
		_client_fd(clientFD),
		_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_body(),
		_flag(NO_FLAG),
		_uri(),
		_headers()
{
	if (this->_client_fd <= STDERR_FILENO)
		throw (ExceptionMaker("Bad client FD for request"));
	this->readClient();
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
void	Request::readClient()
{
	std::ustring::size_type	request_i;
	std::ustring			request;
	std::ustring			chunk;
	std::string				token;

	do
	{
		chunk = this->getNextChunkClient();
		if (request.size() + chunk.size() > Request::_max_request_size)
		{
			this->_flag = _400; 
			// Also add logging
			return ;
		};
		request += chunk;
	} while (!chunk.empty());
	request_i = 0;
	{
		std::stringstream	ss(seekCRLF(request, request_i));

		for (int i = 0; i < 3 && std::getline(ss, token, ' '); i++)
		{
			if (i == 0)
				this->_method = Http::sToMethod(token);
			else if (i == 1)
				this->_uri = token;
			else if (i == 2)
				this->_version = Http::sToVersion(token);
		}
	}
	while (request_i < request.size())
	{
		std::stringstream	ss(seekCRLF(request, request_i));
		if (ss.str() == CRLF)
			break ;
		this->parseHeaderLine(ss);
	}
	this->parseBody(request.substr(request_i));
}

void	Request::parseBody(std::ustring const& body)
{
	std::string	content_length_val;

	if (this->getMethod() == Http::M_POST)
	{
		if (this->getHeader("content-type") != NO_SUCH_HEADER)
		{
			content_length_val = this->getHeader("content-length");
			if (content_length_val != NO_SUCH_HEADER)
			{
				if (std::strtoul(content_length_val.c_str(), NULL, 10) != body.size())
				{
					this->_flag = _400;
					// Also add logging
					return ;
				}
			}
			this->_body = body;
		}
	}
}

std::ustring	Request::getNextChunkClient()
{
	unsigned char	buff[CLIENT_CHUNK_SIZE];
	std::ustring	chunk;
	long			r;

	r = read(this->getClientFD(), buff, CLIENT_CHUNK_SIZE);
	if (r < 0)
		throw (ExceptionMaker(strerror(errno)));
	if (r == 0)
		return (std::ustring());
	chunk.assign(buff, buff + r);
	return (chunk);
}

ServerConfig const&	Request::getServerConfig()	const
{
	return (this->_server_config);
}

Http::VERSION const&	Request::getVersion()	const
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

Http::METHOD const&	Request::getMethod()	const
{
	return (this->_method);
}

Socket const&	Request::getClientFD()	const
{
	return (this->_client_fd);
}

std::string const	Request::getHeader(std::string const& header)
{
	StrStrMap::iterator	it;

	it = this->_headers.find(Utils::lowerStr(header));
	if (it != this->_headers.end())
		return (it->second);
	return (NO_SUCH_HEADER);
}

void	Request::putHeader(std::string const& header, std::string const& val)
{
	this->_headers[Utils::lowerStr(header)] = val;
}

/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
unsigned int const	Request::_max_request_size = CLIENT_CHUNK_SIZE * 2000;

std::string	Request::seekCRLF(std::ustring const& request,
	std::ustring::size_type & index)
{
	std::string s;

	while (index < request.size())
	{
		if (request[index] < 32 || request[index] > 126)
			throw (ExceptionMaker("Invalid request-line"));
		if (index + 1 < request.size()
			&& request[index] == '\r'
			&& request[index + 1] == '\n')
		{
			index += 2;
			break ;
		}
		s += request[index];
	}
	return (s);
}

void	Request::parseHeaderLine(std::stringstream & headerLine)
{
	std::string	key;
	std::string	val;

	std::getline(headerLine, key, ':');
	if (key.empty())
		return ;
	headerLine.ignore(key.size() + 1);
	headerLine >> val;
	this->putHeader(key, val);
}
/**************************************************************************/
