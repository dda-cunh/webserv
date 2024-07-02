#include "../../includes/classes/Response.hpp"

/****************************  CANNONICAL FORM  ****************************/

Response::Response(void)
	: _statusCode(200),
	  _headers(),
	  _body(),
	  _response()
{
}

Response::Response(Response const &src)
	: _statusCode(200),
	  _headers(),
	  _body(),
	  _response()
{
	*this = src;
}

Response &Response::operator=(Response const &rhs)
{
	(void)rhs;
	return (*this);
}

Response::~Response(void)
{
}
/**************************************************************************/

/********************************  HELPERS  *******************************/

std::string to_string(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

bool resourceExists(std::string uri)
{
	std::ifstream file(uri.c_str());
	return file.good();
}

std::string readResource(std::string uri)
{
	std::ifstream file(uri.c_str());
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}

std::string getResourceContentType(std::string uri)
{
	std::string extension = uri.substr(uri.find_last_of(".") + 1);
	if (extension == "html")
		return "text/html";
	else if (extension == "css")
		return "text/css";
	else if (extension == "js")
		return "text/javascript";
	else
		return "text/plain";
}

std::string getCurrentDate()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *gmtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tstruct);
	return buf;
}

/**************************************************************************/

/********************************  MEMBERS  *******************************/

/**
 * @brief Dispatches the request method.
 *
 * Checks the request flag and method, sets the appropriate status code, headers, and body,
 * calls the relevant method handler and sets response string.
 * @param request The request to dispatch.
 */
void Response::dispatchRequestMethod(Request const &request)
{
	if (request.flag() == _400)
	{
		_statusCode = Http::SC_BAD_REQUEST;
		setHeader("Content-Type", "text/plain");
		_body = getHTTPStatus(_statusCode); // Eventually html error pages
	}
	else
	{
		if (request.method() == Http::M_GET)
			this->handleGETMethod(request);
		// more methods...
		else
		{
			_statusCode = Http::SC_METHOD_NOT_ALLOWED;
			setHeader("Allow", "GET");
			setHeader("Content-Type", "text/plain");
			_body = getHTTPStatus(_statusCode); // Eventually html error pages
		}
	}
	setCommonHeaders();
	setResponse();
}

void Response::handleGETMethod(Request const &request)
{
	std::string uri = request.uri();

	if (!uri.empty() && uri[0] == '/')
		uri = uri.substr(1);

	if (resourceExists(uri))
	{
		_statusCode = Http::SC_OK;
		_body = readResource(uri);
		setHeader("Content-Type", getResourceContentType(uri));
	}
	else
	{
		_statusCode = Http::SC_NOT_FOUND;
		setHeader("Content-Type", "text/plain");
		_body = getHTTPStatus(_statusCode);
	}
}

void Response::setCommonHeaders()
{
	setHeader("Server", "webserv/0.1");
	setHeader("Content-Length", to_string(_body.length()));
	setHeader("Date", getCurrentDate());
}

void Response::setHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::setResponse()
{
	_response += "HTTP/1.1 " + to_string(_statusCode) + " " + getHTTPStatus(_statusCode) + CRLF;
	for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		_response += it->first + ": " + it->second + CRLF;
	_response += CRLF + _body;
}

std::string Response::getHTTPStatus(int statusCode) const
{
	switch (statusCode)
	{
	case Http::SC_OK:
		return "OK";
	case Http::SC_BAD_REQUEST:
		return "Bad Request";
	case Http::SC_NOT_FOUND:
		return "Not Found";
	case Http::SC_METHOD_NOT_ALLOWED:
		return "Method Not Allowed";
	default:
		return "Internal Server Error";
	}
}

std::string const &Response::response() const
{
	return _response;
}