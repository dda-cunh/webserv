#include "../../includes/classes/Response.hpp"

/****************************  CANNONICAL FORM  ****************************/

Response::Response(void)
	: _statusCode(200),
	  _headers(),
	  _body(),
	  _response(),
	  _request(Request(0)),
	  _error_pages()
{
}

Response::Response(Response const &src)
	: _statusCode(200),
	  _headers(),
	  _body(),
	  _response(),
	  _request(Request(0)),
	  _error_pages()
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

/*****************************  CONSTRUCTORS  *****************************/

Response::Response(Request const &request)
	: _statusCode(200),
	  _headers(),
	  _body(),
	  _response(),
	  _request(request)
{
	setErrorPages();
	dispatchRequestMethod();
}

/**************************************************************************/

/**************************  HELPERS  / DEBUG  ****************************/

std::string to_string(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

bool resourceExists(std::string const &uri)
{
	std::ifstream file(uri.c_str());
	return file.good();
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
	else if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	else if (extension == "png")
		return "image/png";
	else if (extension == "gif")
		return "image/gif";
	else if (extension == "svg")
		return "image/svg+xml";
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

// TODO: Remove this dummy function once config can be extracted from elsewhere
IntStrMap dummyGetErrorPages()
{
	IntStrMap error_pages;

	error_pages[Http::SC_OK] = "tests/error_pages/200.html";
	error_pages[Http::SC_CREATED] = "tests/error_pages/201.html";
	error_pages[Http::SC_NO_CONTENT] = "tests/error_pages/204.html";
	error_pages[Http::SC_BAD_REQUEST] = "tests/error_pages/400.html";
	error_pages[Http::SC_FORBIDDEN] = "tests/error_pages/403.html";
	error_pages[Http::SC_NOT_FOUND] = "tests/error_pages/404.html";
	error_pages[Http::SC_METHOD_NOT_ALLOWED] = "tests/error_pages/405.html";
	error_pages[Http::SC_CONFLICT] = "tests/error_pages/409.html";
	error_pages[Http::SC_INTERNAL_SERVER_ERROR] = "tests/error_pages/500.html";
	error_pages[Http::SC_NOT_IMPLEMENTED] = "tests/error_pages/501.html";
	error_pages[Http::SC_BAD_GATEWAY] = "tests/error_pages/502.html";
	error_pages[Http::SC_SERVICE_UNAVAILABLE] = "tests/error_pages/503.html";
	error_pages[Http::SC_VERSION_NOT_SUPPORTED] = "tests/error_pages/505.html";

	return error_pages;
}

std::string Response::getResponseWithoutBody() const // Debug function, to be removed
{
	std::string responseWithoutBody;
	responseWithoutBody += "HTTP/1.1 " + to_string(_statusCode) + " " + getHTTPStatus(_statusCode) + CRLF;
	for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		responseWithoutBody += it->first + ": " + it->second + CRLF;
	responseWithoutBody += CRLF;
	return responseWithoutBody;
}

bool is_directory(std::string uri)
{
	struct stat path_stat;
	stat(uri.c_str(), &path_stat);
	return S_ISDIR(path_stat.st_mode);
}

std::string constructUri(std::string uri, std::string root)
{
	if (uri == "/")
		return root;
	else
		return root + "/" + uri;
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
void Response::dispatchRequestMethod()
{
	if (_request.flag() == _400)
	{
		_statusCode = Http::SC_BAD_REQUEST;
		readResource(_error_pages[_statusCode]);
	}
	else
	{
		if (_request.method() == Http::M_GET)
			this->handleGETMethod(_request);
		// more methods...
		else
		{
			_statusCode = Http::SC_METHOD_NOT_ALLOWED;
			setHeader("Allow", "GET");
			readResource(_error_pages[_statusCode]);
		}
	}
	setCommonHeaders();
	setResponse();
	Utils::log("Response:", Utils::LOG_INFO);
	Utils::log(getResponseWithoutBody(), Utils::LOG_INFO);
}

void Response::handleGETMethod(Request const &request)
{
	std::string uri = request.uri();
	std::string root = "test_files/www";	  // TODO: get this from config
	bool is_directory_listing_enabled = true; // TODO: get this from config

	uri = constructUri(uri, root);

	if (is_directory(uri))
		handleDirectory(uri, is_directory_listing_enabled);
	else if (resourceExists(uri))
		readResource(uri);
	else
		handleNotFound(uri);
}

void Response::handleDirectory(std::string uri, bool is_directory_listing_enabled)
{
	if (uri[uri.size() - 1] == '/')
	{
		if (is_directory_listing_enabled)
			listDirectory(uri);
		else
			handleDirectoryDefaultFile(uri);
	}
	else
	{
		handleDirectoryDefaultFile(uri);
	}
}

void Response::handleDirectoryDefaultFile(std::string uri)
{
	if (resourceExists(uri + "/index.html"))
		readResource(uri + "/index.html");
	else
		handleNotFound(uri);
}

void Response::handleNotFound(std::string uri)
{
	_statusCode = Http::SC_NOT_FOUND;
	uri = _error_pages[_statusCode];
	readResource(uri);
}

void Response::listDirectory(std::string &uri) //TODO: return html
{
	DIR *dir;
	struct dirent *ent;
	std::string dir_listing;

	if ((dir = opendir(uri.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			dir_listing += ent->d_name;
			dir_listing += "\n";
		}
		closedir(dir);
	}
	else
	{
		_statusCode = Http::SC_INTERNAL_SERVER_ERROR;
		uri = _error_pages[_statusCode];
		readResource(uri);
		return;
	}

	_body = dir_listing;
	setHeader("Content-Type", "text/plain");
}

/**************************************************************************/

/*******************************  SETTERS  *******************************/

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

void Response::setErrorPages()
{
	_error_pages = dummyGetErrorPages();
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
void Response::readResource(std::string uri)
{
	std::ifstream file(uri.c_str(), std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	setHeader("Content-Type", getResourceContentType(uri));
	_body = content;
}
std::string const &Response::response() const
{
	return _response;
}