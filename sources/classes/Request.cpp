#include "../../includes/classes/Request.hpp"
#include <string>

#define FLAG_REQ(flag)								\
	{												\
			this->_parsing_stage = REQ_PARSED_BODY;	\
			this->_flag = _400;						\
			return ;								\
	}

/****************************  CANNONICAL FORM  ****************************/
Request::Request(void)
	:	_parsing_stage(REQ_PARSED_NONE),
		_chunk_len(0),
		_version(Http::V_UNHANDLED),
		_method(Http::M_UNHANDLED),
		_flag(NO_FLAG),
		_parse_feed(),
		_body(),
		_uri(),
		_headers(),
		_content_len(0)
{}

Request::Request(Request const & src)
{
	*this = src;
}

Request & Request::operator=(Request const & rhs)
{
	this->_parsing_stage = rhs.parsingStage();
	this->_version = rhs.version();
	this->_method = rhs.method();
	this->_flag = rhs.flag();
	this->_parse_feed = rhs._parse_feed;
	this->_uri = rhs.uri();
	this->_headers = rhs._headers;
	this->_body = rhs.body();
	return (*this);
}

Request::~Request(void)
{}
/**************************************************************************/

/********************************  MEMBERS  *******************************/
std::istream &	Request::getCRLF(std::istream & input, std::string & output)
{
	char		curr;

	output.clear();
	while (input.get(curr))
	{
		if (curr == '\n' && !output.empty() && output[output.length() - 1] == '\r')
		{
			output = output.substr(0, output.length() - 1);
			break;
		}
		output += curr;
	}
	return input;
}

void	Request::doParse(std::string requestBytes)
{
	std::stringstream	request_ss(this->_parse_feed + requestBytes);
	std::string			line;

	this->_parse_feed.clear();
	if (this->_parsing_stage ==  REQ_PARSED_BODY)
		return ;
	if (this->_parsing_stage == REQ_PARSED_NONE)
	{
		std::stringstream	firstLine_ss;
		std::string			token;

		if (requestBytes.empty())
			FLAG_REQ(_EMPTY);
		if (!this->getCRLF(request_ss, line))
			FLAG_REQ(_400);
		firstLine_ss << line;
		for (int i = 0; i < 3 && std::getline(firstLine_ss, token, ' '); i++)
		{
			if (i == 0)
				this->_method = Http::sToMethod(token);
			else if (i == 1)
				this->_uri = token;
			else if (i == 2)
				this->_version = Http::sToVersion(token);
		}
		if (this->_method == Http::M_UNHANDLED
			|| this->_version == Http::V_UNHANDLED)
			FLAG_REQ(_400);
		this->_parsing_stage = REQ_PARSED_FIRST_LINE;
	}
	if (this->_parsing_stage == REQ_PARSED_FIRST_LINE)
	{
		while (true)
		{
			if (!this->getCRLF(request_ss, line))
			{
				this->_parse_feed = line;
				break ;
			}
			if (line.empty())
			{
				this->_parsing_stage = REQ_PARSED_HEADERS;
				break ;
			}
			else 
				this->parseHeaderLine(line);
		}
		if (this->_parsing_stage == REQ_PARSED_HEADERS)
		{
			if (this->header("content-length") != Request::_no_such_header)
				this->_content_len = std::strtoul(this->header("content-length").c_str(),
													NULL, 10);
			if (this->header("transfer-encoding") != Request::_no_such_header)
			{
				if (this->_content_len > 0)
					FLAG_REQ(_400);
				if (this->header("transfer-encoding") == "chunked")
					this->_content_len = TRANSFER_ENCODING_CHUNKED;
				else
					FLAG_REQ(_400);
			}
		}
	}
	if (this->_parsing_stage == REQ_PARSED_HEADERS)
	{
		if (this->parseBody(request_ss))
			this->_parsing_stage = REQ_PARSED_BODY;
	}
}

bool	Request::parseBody(std::stringstream & bodyBytes)
{
	if (this->method() != Http::M_GET)
	{
		if (this->_content_len > 0)
		{
			this->_body += bodyBytes.str().substr(bodyBytes.tellg());
			if (this->_body.length() < (unsigned long)this->_content_len)
				return (false);
		}
		else if (this->_content_len == TRANSFER_ENCODING_CHUNKED)
		{
			std::string	line;

			if (!getCRLF(bodyBytes, line))
			{
				this->_parse_feed = line;
				return (false);
			}
			if (this->_chunk_len == 0)
			{
				if (line == "0")
					return (true);
				this->_chunk_len = std::strtoul(line.c_str(), NULL, 16);
			}
			if (!getCRLF(bodyBytes, line))
			{
				this->_parse_feed = line;
				return (false);
			}
			this->_body += line;
			this->_chunk_len -= line.length();
			if (this->_chunk_len != 0)
			{
				this->_flag = _400;
				return (true);
			}
			return (false);
		}
	}
	return (true);
}

ReqParsedStage const&	Request::parsingStage()	const
{
	return (this->_parsing_stage);
}

Http::VERSION const&	Request::version()	const
{
	return (this->_version);
}

std::string const&	Request::body()	const
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
std::string const	Request::_no_such_header = std::string();
/**************************************************************************/

