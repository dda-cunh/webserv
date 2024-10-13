#include "../../includes/classes/Response.hpp"
#include <sstream> // for std::ostringstream
#include <sys/wait.h>

/**************************************************************************/

/****************************  CANNONICAL FORM  ****************************/

Response::~Response()
{
}

/**************************************************************************/

/**************************  HELPERS  / DEBUG  ****************************/

std::string getResourceContentType(std::string uri)
{
	std::string extension = uri.substr(uri.find_last_of(".") + 1);
	if (extension == "html")
		return "text/html";
	if (extension == "css")
		return "text/css";
	if (extension == "js")
		return "text/javascript";
	if (extension == "jpg" || extension == "jpeg")
		return "image/jpeg";
	if (extension == "png")
		return "image/png";
	if (extension == "gif")
		return "image/gif";
	if (extension == "svg")
		return "image/svg+xml";
	return "text/plain";
}

/**
 * @brief Generates the HTTP response headers string.
 * @return A string containing the HTTP response headers.
 */
std::string Response::getHeadersStr()
{
    std::string responseHeaders;
    responseHeaders += "HTTP/1.1 " + Utils::intToString(_statusCode) + " " + Http::sToReasonPhrase(_statusCode) + CRLF;
    for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        responseHeaders += it->first + ": " + it->second + CRLF;
    responseHeaders += CRLF;
    return responseHeaders;
}

std::string extractFileNameFromQuery(const std::string &uri)
{
	std::string fileName;
	size_t queryStartPos = uri.find('?');
	if (queryStartPos != std::string::npos)
	{
		std::string queryString = uri.substr(queryStartPos + 1);
		size_t fileParamPos = queryString.find("file=");
		if (fileParamPos != std::string::npos)
		{
			fileName = queryString.substr(fileParamPos + 5);
		}
	}
	return fileName;
}

/**
 * @brief Sets the best matching location block for the requested URI.
 */
void Response::setMatchedLocation() {
    ServerLocation *bestMatch = NULL;
    std::string longestMatch;

    for (size_t i = 0; i < _serverBlocks.size(); ++i) {
        const ServerConfig &config = _serverBlocks[i];

        for (size_t j = 0; j < config.getLocationBlocksSize(); ++j) {
            ServerLocation *location = config.getLocationFromIndex(j);
            std::string locationPath = location->getLocation();

            if (_request.uri().find(locationPath) == 0 && locationPath.size() > longestMatch.size()) {
                longestMatch = locationPath;
                bestMatch = location;
            }
        }
    }

    if (bestMatch == NULL) {
        throw std::runtime_error("No valid location block found for the requested URI.");
    }

    _matchedLocation = bestMatch;

    std::cout << "Matched location: " << *static_cast<LocationStatic*>(_matchedLocation) << std::endl;
}


/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/

Response::Response(Request const &request, ServerBlocks const &server_blocks)
    : _statusCode(Http::SC_OK),
      _headers(),
      _body(),
      _response(),
      _request(request),
      _serverBlocks(server_blocks)
{
	std::string body(request.body().begin(), request.body().end());

    try {
		setMatchedLocation();
	} catch (std::exception &e) {
		Utils::log(e.what(), Utils::LOG_ERROR);
		setStatusAndReadResource(Http::SC_NOT_FOUND);
	}
	
    if (_request.flag() == _400) {
        setStatusAndReadResource(Http::SC_BAD_REQUEST);
    } else if (_request.method() == Http::M_UNHANDLED) {
        setStatusAndReadResource(Http::SC_NOT_IMPLEMENTED);
    } else if (isRedirection()) {
		handleRedirection();
    } else {
        dispatchMethod();
    }

    setCommonHeaders();
    setResponse();
    Utils::log("Response:", Utils::LOG_INFO);
    Utils::log(getHeadersStr(), Utils::LOG_INFO);
}

/**************************************************************************/

/********************************  MEMBERS  *******************************/

/**
 * @brief Dispatches the request method.
 *
 * Calls the relevant method handler.
 */
void Response::dispatchMethod()
{
    if (!_matchedLocation->methodIsAllowed(_request.method()))
        handleMethodNotAllowed();
	else if (_request.uri().find(".cgi") != std::string::npos)
		handleCGI();
    else if (_request.method() == Http::M_GET)
        handleGETMethod();
}

/**
 * @brief  Serves the requested file or directory listing.
 */
void Response::handleGETMethod()
{
	std::string root = _matchedLocation->getRootDir();
	bool autoindex = static_cast<LocationStatic*>(_matchedLocation)->getAutoIndex();

	std::string uri = (_request.uri() == "/") ? root : Utils::concatenatePaths(root, _request.uri());

	if (Directory::isDirectory(uri))
	{
		Directory::Result result = Directory::handleDirectory(uri, autoindex);
		_statusCode = result.statusCode;
		uri = _statusCode == Http::SC_OK ? result.path : _matchedLocation->getErrPagePath(_statusCode);
		readResource(uri);
	}
	else if (Utils::resourceExists(uri))
	{
		readResource(uri);
	}
	else
	{
		setStatusAndReadResource(Http::SC_NOT_FOUND);
	}
}

void Response::handleMethodNotAllowed()
{
    _statusCode = Http::SC_METHOD_NOT_ALLOWED;
    std::string allowedMethodsStr;
    for (size_t i = 0; i < _matchedLocation->getMethodsAllowedSize(); ++i)
        allowedMethodsStr += Http::methodToString(_matchedLocation->getMethodByIndex(i)) + (i < _matchedLocation->getMethodsAllowedSize() - 1 ? ", " : "");
    setHeader("Allow", allowedMethodsStr);
    readResource(_matchedLocation->getErrPagePath(_statusCode));
}

void Response::readResource(const std::string &uri, bool isErrorResponse)
{
	try
	{
		std::ifstream file(uri.c_str(), std::ios::binary);
		if (!file.is_open())
			throw ExceptionMaker("Failed to open file: " + uri);

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (file.bad())
			throw ExceptionMaker("Error reading file: " + uri);

		setHeader("Content-Type", getResourceContentType(uri));
		_body = content;
	}
	catch (ExceptionMaker &e)
	{
		e.log();
		if (!isErrorResponse)
		{
			setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
		}
		else
		{
			Utils::log("Failed to load the internal server error page.", Utils::LOG_ERROR);
			_body = "<html><body><h1>500 Internal Server Error</h1></body></html>";
			setHeader("Content-Type", "text/html");
		}
	}
}

void Response::handleCGI() {
    std::string uri = _request.uri();
    std::string rootDir = _matchedLocation->getRootDir();
    size_t cgiPos = uri.find(".cgi");
    std::string cgiPath = rootDir + uri.substr(0, cgiPos + 4);

	std::vector<std::string> envVars;
	setEnvironmentVariables(cgiPath, _request, envVars);
	std::cout << "handleCGI Environment variables:" << std::endl;
	for (std::vector<std::string>::iterator it = envVars.begin(); it != envVars.end(); ++it)
		std::cout << *it << std::endl;

    int inputPipe[2], outputPipe[2];
    try
    {
        createPipes(inputPipe, outputPipe);

        pid_t pid = fork();
        if (pid == 0)
            handleChildProcess(inputPipe, outputPipe, cgiPath, envVars);
        else if (pid > 0)
            handleParentProcess(inputPipe, outputPipe, _request, *this, pid);
        else
            throw ExceptionMaker("Fork failed: " + std::string(strerror(errno)));
    }
    catch (ExceptionMaker &e)
    {
        e.log();
        setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
    }
}

void Response::setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri)
{
	_statusCode = statusCode;
	if (!uri.empty())
		readResource(uri);
	else
		readResource(_matchedLocation->getErrPagePath(_statusCode), true);
}

bool Response::isRedirection() {
    std::string requestPath = _request.uri();
    std::string locationPath = _matchedLocation->getLocation();

    if (requestPath.find(locationPath) == 0) {
        std::string relativePath = requestPath.substr(locationPath.size());

        StrStrMap::const_iterator it = _matchedLocation->getRedirectionIttBegin();
        StrStrMap::const_iterator end = _matchedLocation->getRedirectionIttEnd();

        for (; it != end; ++it) {
            if (relativePath == it->first) {
                _redirectionPath = locationPath + it->second;
                return true;
            }
        }
    }
    return false;
}

void Response::handleRedirection() {
	if (!_redirectionPath.empty())
	{
		_statusCode = Http::SC_FOUND;
		setHeader("Location", _redirectionPath);
	}
}

/**************************************************************************/

/*******************************  GETTERS  *******************************/

std::string const &Response::getResponse() const
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

void Response::setBody(const std::string &body)
{
	_body = body;
}

/**
 * @brief Sets the complete HTTP response including headers and body.
 */
void Response::setResponse()
{
    _response = getHeadersStr() + _body;
}