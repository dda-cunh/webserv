#include "../../includes/classes/Request.hpp"

/****************************  CANNONICAL FORM  ****************************/
Request::Request(void)
	:	_client_fd(-1),
		_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_flag(NO_FLAG),
		_uri(),
		_headers(),
		_body()
{}

Request::Request(Request const & src)
	:	_client_fd(src._client_fd)
{
	*this = src;
}

Request & Request::operator=(Request const & rhs)
{
	this->_version = rhs.version();
	this->_method = rhs.method();
	this->_flag = rhs.flag();
	this->_uri = rhs.uri();
	this->_headers = rhs._headers;
	this->_body = rhs.body();
	return (*this);
}

Request::~Request(void)
{}
/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/
Request::Request(int const& clientFD)
	:	_client_fd(clientFD),
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
std::string	Request::seekCRLF(ByteArr const& request,
	ByteArr::size_type & index)
{
	std::string s;

	while (index < request.size())
	{
		if (index + 1 < request.size()
			&& request[index] == '\r'
			&& request[index + 1] == '\n')
		{
			index += 2;
			break ;
		}
		if (request[index] < 32 || request[index] > 126)
			throw (ExceptionMaker("Invalid request-line"));
		s += request[index];
		index++;
	}
	return (s);
}

void	Request::readClient()
{
	ByteArr::size_type	request_i;
	std::stringstream	ss;
	std::string			token;
	ByteArr				request;
	ByteArr				chunk;

	do
	{
		chunk = this->getNextChunkClient();
		if (request.size() + chunk.size() > Request::_max_request_size)
		{
			this->_flag = _400; 
			LOG("Request is too big", Utils::LOG_WARNING);
			return ;
		}
		request.insert(request.end(), chunk.begin(), chunk.end());
	} while (!chunk.empty());
	if (request.empty())
	{
		this->_flag = _400;	//	USE A DEDICATED FLAG FOR REQUESTS WITH NO HTTP
		return ;
	}
	request_i = 0;
	ss << this->seekCRLF(request, request_i);
	for (int i = 0; i < 3 && std::getline(ss, token, ' '); i++)
	{

		if (i == 0)
			this->_method = Http::sToMethod(token);
		else if (i == 1)
			this->_uri = token;
		else if (i == 2)
			this->_version = Http::sToVersion(token);
	}
	ss.str(std::string());
	while (request_i < request.size())
	{
		std::string	line;

		line = this->seekCRLF(request, request_i);
		if (line.empty())
			break ;
		this->parseHeaderLine(line);
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
					LOG("Content length doesn't match body length", Utils::LOG_WARNING);
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

	r = recv(this->clientFD(), buff, CLIENT_CHUNK_SIZE, MSG_DONTWAIT);
	if (r <= 0)
		return (ByteArr());
	chunk.assign(buff, buff + r);
	return (chunk);
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

std::string	Request::header(std::string const& header)	const
{
	StrStrMap::const_iterator	it;

	it = this->_headers.find(Utils::lowerStr(header));
	if (it != this->_headers.end())
		return (it->second);
	return (Request::_no_such_header);
}

std::string	Request::str()	const
{
	std::string	s;

	s = "\n\t\t|" + Http::methodToString(this->method()) + " " + this->uri() + " "
		+ Http::versionToString(this->version()) + "|\n";
	for (StrStrMap::const_iterator it = this->_headers.begin();
			it != this->_headers.end(); it++)
		s += "\t\t|" + it->first + ": " + it->second + "|\n";
	s += "\n";
	return (s);
}

void	Request::putHeader(std::string const& key, std::string const& val)
{
	this->_headers.insert(StrStrMap::value_type(Utils::lowerStr(key), val));
}

void	Request::parseHeaderLine(std::string const& headerLine)
{
	std::stringstream	ss(headerLine);
	std::string			key;
	std::string			val;

	std::getline(ss, key, ':');
	if (key.empty())
		return ;
	std::getline(ss, val);
	this->putHeader(key, Utils::sTrim(val));
}

/**************************************************************************/

/*****************************  STATIC MEMBERS  ***************************/
unsigned int const	Request::_max_request_size = CLIENT_CHUNK_SIZE * 2000;
std::string const	Request::_no_such_header = std::string();
/**************************************************************************/

