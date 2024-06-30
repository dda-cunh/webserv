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
Request::Request(ServerConfig const& config, int const& clientFD)
	:	_server_config(config),
		_client_fd(clientFD),
		_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_flag(NO_FLAG),
		_uri(),
		_headers(),
		_body()
{
	if (this->_client_fd <= STDERR_FILENO)
		throw (ExceptionMaker("Bad client FD for request"));
	this->readClient();
}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
void	Request::readClient()
{
	ByteArr::size_type	request_i;
	std::string			token;
	ByteArr				request;
	ByteArr				chunk;

	do
	{
		chunk = this->getNextChunkClient();
		if (request.size() + chunk.size() > Request::_max_request_size)
		{
			this->_flag = _400; 
			Utils::log("Request is too big", Utils::LOG_WARNING);
			return ;
		};
		request.insert(request.end(), chunk.begin(), chunk.end());
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
		if (ss.str().empty())
			break ;
		this->parseHeaderLine(ss);
	}
	this->parseBody(ByteArr(request.begin() + request_i, request.end()));
}

void	Request::parseBody(ByteArr const& body)
{
	std::string	content_length_val;

	if (this->method() == Http::M_POST)
	{
		if (this->header("content-type") != Request::_no_such_header)
		{
			content_length_val = this->header("content-length");
			if (content_length_val != Request::_no_such_header)
			{
				if (std::strtoul(content_length_val.c_str(), NULL, 10) != body.size())
				{
					this->_flag = _400;
					Utils::log("Content length doesn't match body length", Utils::LOG_WARNING);
					return ;
				}
			}
			this->_body = body;
		}
	}
}

ByteArr	Request::getNextChunkClient()
{
	unsigned char	buff[CLIENT_CHUNK_SIZE];
	ByteArr			chunk;
	long			r;

	r = read(this->clientFD(), buff, CLIENT_CHUNK_SIZE);
	if (r < 0)
		throw (ExceptionMaker(strerror(errno)));
	if (r == 0)
		return (ByteArr());
	chunk.assign(buff, buff + r);
	return (chunk);
}

ServerConfig const&	Request::serverConfig()	const
{
	return (this->_server_config);
}

Http::VERSION const&	Request::version()	const
{
	return (this->_version);
}

ByteArr const&	Request::body()	const
{
	return (this->_body);
}

ResponseFlag const&			Request::flag()	const
{
	return (this->_flag);
}


std::string const&	Request::uri()	const
{
	return (this->_uri);
}

Http::METHOD const&	Request::method()	const
{
	return (this->_method);
}

int const&	Request::clientFD()	const
{
	return (this->_client_fd);
}

std::string const	Request::header(std::string const& header)
{
	StrStrMap::iterator	it;

	it = this->_headers.find(Utils::lowerStr(header));
	if (it != this->_headers.end())
		return (it->second);
	return (Request::_no_such_header);
}

void	Request::putHeader(std::string const& header, std::string const& val)
{
	this->_headers[Utils::lowerStr(header)] = val;
}

/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
unsigned int const	Request::_max_request_size = CLIENT_CHUNK_SIZE * 2000;
std::string const	Request::_no_such_header = std::string();

std::string	Request::seekCRLF(ByteArr const& request,
	ByteArr::size_type & index)
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
