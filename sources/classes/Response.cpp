#include "../../includes/classes/Response.hpp"

/**************************************************************************/

/****************************  CANNONICAL FORM  ****************************/

Response::Response(void)
	: _statusCode(Http::SC_OK),
	  _headers(),
	  _body(),
	  _response(),
	  _request(Request(0)),
	  _error_pages()
{
}

Response::Response(Response const &src)
	: _statusCode(Http::SC_OK),
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

/**************************  HELPERS  / DEBUG  ****************************/

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

// TODO: Get this from config and remove function
IntStrMap dummyGetErrorPages()
{
	IntStrMap error_pages;

	error_pages[Http::SC_OK] = "test_files/error_pages/200.html";
	error_pages[Http::SC_CREATED] = "test_files/error_pages/201.html";
	error_pages[Http::SC_NO_CONTENT] = "test_files/error_pages/204.html";
	error_pages[Http::SC_BAD_REQUEST] = "test_files/error_pages/400.html";
	error_pages[Http::SC_FORBIDDEN] = "test_files/error_pages/403.html";
	error_pages[Http::SC_NOT_FOUND] = "test_files/error_pages/404.html";
	error_pages[Http::SC_METHOD_NOT_ALLOWED] = "test_files/error_pages/405.html";
	error_pages[Http::SC_CONFLICT] = "test_files/error_pages/409.html";
	error_pages[Http::SC_INTERNAL_SERVER_ERROR] = "test_files/error_pages/500.html";
	error_pages[Http::SC_NOT_IMPLEMENTED] = "test_files/error_pages/501.html";
	error_pages[Http::SC_BAD_GATEWAY] = "test_files/error_pages/502.html";
	error_pages[Http::SC_SERVICE_UNAVAILABLE] = "test_files/error_pages/503.html";
	error_pages[Http::SC_VERSION_NOT_SUPPORTED] = "test_files/error_pages/505.html";

	return error_pages;
}

std::string Response::getResponseWithoutBody() // Debug function, to be removed
{
	std::string responseWithoutBody;
	responseWithoutBody += "HTTP/1.1 " + Utils::intToString(_statusCode) + " " + Http::sToReasonPhrase(_statusCode) + CRLF;
	for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		responseWithoutBody += it->first + ": " + it->second + CRLF;
	responseWithoutBody += CRLF;
	return responseWithoutBody;
}

std::string constructUri(std::string uri, std::string root)
{
	if (uri == "/")
		return root;
	else
		return root + "/" + uri;
}

/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/

Response::Response(Request const &request)
	: _statusCode(Http::SC_OK),
	  _headers(),
	  _body(),
	  _response(),
	  _request(request)
{
	setErrorPages();
	dispatchRequestMethod();
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
		// TODO: get allowed methods from config, return 405 if not allowed
		if (_request.method() == Http::M_GET)
			this->handleGETMethod(_request);
		else if (_request.method() == Http::M_POST)
			this->handlePOSTMethod(_request);
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

// TODO: the file content parsing should be called by the request parsing logic, not the response as it's being done now
void Response::handlePOSTMethod(Request const &request)
{
	if (request.header("content-type").find("multipart/form-data") == std::string::npos)
	{
		_statusCode = Http::SC_BAD_REQUEST;
		readResource(_error_pages[_statusCode]);
		return;
	}

	std::string fileName;
	std::string fileContent;
	if (!request.parseFileContent(fileName, fileContent))
	{
		Utils::log("Error parsing file content", Utils::LOG_ERROR);
		_statusCode = Http::SC_BAD_REQUEST;
		readResource(_error_pages[_statusCode]);
		return;
	}

	std::string filePath = "test_files/uploaded_files/" + fileName; // TODO: Get this from config

	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (!outFile)
	{
		Utils::log("Error opening file for writing", Utils::LOG_ERROR);
		_statusCode = Http::SC_INTERNAL_SERVER_ERROR;
		readResource(_error_pages[_statusCode]);
		return;
	}

	outFile.write(fileContent.data(), fileContent.size());
	outFile.close();

	if (!outFile)
	{
		Utils::log("Error writing to file", Utils::LOG_ERROR);
		_statusCode = Http::SC_INTERNAL_SERVER_ERROR;
		readResource(_error_pages[_statusCode]);
		return;
	}

	_statusCode = Http::SC_CREATED;
	setHeader("Location", filePath);
	_body = "File uploaded successfully"; // TODO: Return HTML in the body
}

void Response::handleGETMethod(Request const &request)
{
	std::string uri = request.uri();
	std::string root = "test_files/www"; // TODO: get this from config
	bool autoindex = true;				 // TODO: get this from config

	uri = constructUri(uri, root);

	if (Utils::isDirectory(uri))
	{
		Directory::Result result = Directory::handleDirectory(uri, autoindex);
		_statusCode = result.statusCode;
		uri = result.path;
		if (_statusCode == Http::SC_OK)
			readResource(uri);
		else
			readResource(_error_pages[_statusCode]);
	}
	else if (Utils::resourceExists(uri))
	{
		readResource(uri);
	}
	else
	{
		_statusCode = Http::SC_NOT_FOUND;
		readResource(_error_pages[_statusCode]);
	}
}

void Response::readResource(std::string uri)
{
	std::ifstream file(uri.c_str(), std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	setHeader("Content-Type", getResourceContentType(uri));
	_body = content;
}

/**************************************************************************/

/*******************************  GETTERS  *******************************/

std::string const &Response::response() const
{
	return _response;
}

/**************************************************************************/

/*******************************  SETTERS  *******************************/

void Response::setCommonHeaders()
{
	setHeader("Server", "webserv/0.1");
	setHeader("Content-Length", Utils::intToString(_body.length()));
	setHeader("Date", Utils::getCurrentDate());
}

void Response::setHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::setResponse()
{
	_response += "HTTP/1.1 " + Utils::intToString(_statusCode) + " " + Http::sToReasonPhrase(_statusCode) + CRLF;
	for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		_response += it->first + ": " + it->second + CRLF;
	_response += CRLF + _body;
}

void Response::setErrorPages()
{
	_error_pages = dummyGetErrorPages();
}
