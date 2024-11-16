#include "../../includes/classes/Request.hpp"

/****************************  CANNONICAL FORM  ****************************/
Request::Request(void)
	:	_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_flag(NO_FLAG),
		_uri(),
		_headers(),
		_body()
{}

Request::Request(Request const & src)
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
Request::Request(ByteArr const& requestBytes)
	:	_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_flag(NO_FLAG),
		_uri(),
		_headers(),
		_body()
{
	ByteArr::size_type	request_i;
	std::stringstream	ss;
	std::string			token;
	ByteArr				chunk;

	if (requestBytes.empty())
	{
		this->_flag = _EMPTY;
		return ;
	}
	request_i = 0;
	ss << this->seekCRLF(requestBytes, request_i);
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
	while (request_i < requestBytes.size())
	{
		std::string	line;

		line = this->seekCRLF(requestBytes, request_i);
		if (line.empty())
			break ;
		this->parseHeaderLine(line);
	}
	this->parseBody(ByteArr(requestBytes.begin() + request_i, requestBytes.end()));
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

