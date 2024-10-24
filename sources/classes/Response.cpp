#include "../../includes/classes/Response.hpp"
#include <sstream> // for std::ostringstream
#include <sys/wait.h>

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

    _locationMatch = bestMatch;
    Utils::log("Matched location:", Utils::LOG_INFO);
    std::cout << *_locationMatch << std::endl;
}


/**************************************************************************/

/**********************  CONSTRUCTORS / DESTRUCTORS ***********************/

Response::~Response()
{
}

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
    Utils::log("Post dispatch headers: ", Utils::LOG_INFO);
    std::cout << "\t" << getHeadersStr() << std::endl;
    setCommonHeaders();
    setResponse();
    Utils::log("Final headers:", Utils::LOG_INFO);
    std::cout << "\t" << getHeadersStr() << std::endl;
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
    if (!_locationMatch->methodIsAllowed(_request.method()))
        handleMethodNotAllowed();
    else if (setCGIMatch(), !_cgiMatch.getBinary().empty())
        handleCGI();
    else if (_request.method() == Http::M_GET)
        handleGETMethod();
}

void Response::handleMethodNotAllowed()
{
    _statusCode = Http::SC_METHOD_NOT_ALLOWED;
    std::string allowedMethodsStr;
    for (size_t i = 0; i < _locationMatch->getMethodsAllowedSize(); ++i)
        allowedMethodsStr += Http::methodToString(_locationMatch->getMethodByIndex(i)) + (i < _locationMatch->getMethodsAllowedSize() - 1 ? ", " : "");
    setHeader("Allow", allowedMethodsStr);
    readResource(_locationMatch->getErrPagePath(_statusCode));
}

void Response::handleCGI() {
    std::string uri = _request.uri();
    std::string cgiPath = Utils::concatenatePaths(_locationMatch->getRootDir().c_str(), _cgiMatch.getBasePath().c_str(), _cgiMatch.getScriptName().c_str(), NULL);
    
    if (access(cgiPath.c_str(), X_OK) != 0) {
        setStatusAndReadResource(Http::SC_NOT_FOUND);
        return;
    }

    std::vector<std::string> envVars;
    setEnvironmentVariables(cgiPath, *this, envVars);

    Utils::log("CGI process env variables::", Utils::LOG_INFO);
    for (size_t i = 0; i < envVars.size(); ++i) {
        std::cout << "\t" << envVars[i] << std::endl;
    }

    int inputPipe[2], outputPipe[2];

    try {
        createPipes(inputPipe, outputPipe);
        pid_t pid = fork();
        if (pid == 0)
            handleChildProcess(inputPipe, outputPipe, *this, envVars);
        else if (pid > 0)
            handleParentProcess(inputPipe, outputPipe, _request, *this, pid);
        else
            throw ExceptionMaker("Fork failed: " + std::string(strerror(errno)));
    } catch (ExceptionMaker &e) {
        e.log();
        setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
    }
}

/**
 * @brief  Serves the requested file or directory listing.
 */
void Response::handleGETMethod()
{
	std::string root = _locationMatch->getRootDir();
	bool autoindex = _locationMatch->getAutoIndex();

    std::string uri = (_request.uri() == "/") ? root : Utils::concatenatePaths(root.c_str(), _request.uri().c_str(), NULL);
    
	if (Directory::isDirectory(uri))
	{
		Directory::Result result = Directory::handleDirectory(uri, autoindex);
		_statusCode = result.statusCode;
		uri = _statusCode == Http::SC_OK ? result.path : _locationMatch->getErrPagePath(_statusCode);
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

void Response::setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri)
{
	_statusCode = statusCode;
	if (!uri.empty())
		readResource(uri);
	else
		readResource(_locationMatch->getErrPagePath(_statusCode), true);
}

bool Response::isRedirection() {
    std::string requestPath = _request.uri();
    std::string locationPath = _locationMatch->getLocation();

    if (requestPath.find(locationPath) == 0) {
        std::string relativePath = requestPath.substr(locationPath.size());

        StrStrMap::const_iterator it = _locationMatch->getRedirectionIttBegin();
        StrStrMap::const_iterator end = _locationMatch->getRedirectionIttEnd();

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


ServerLocation *Response::getLocationMatch()
{
	return _locationMatch;
}

Request const &Response::getRequest() const
{
    return _request;
}

CGIMatch const &Response::getCGIMatch() const
{
    return _cgiMatch;
}

/**************************************************************************/

/*******************************  SETTERS  *******************************/

void Response::setCommonHeaders()
{
	if (_headers.find("Content-Length") == _headers.end())
		setHeader("Content-Length", Utils::intToString(_body.length()));
	setHeader("Server", "webserv/0.1");
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

void Response::setStatusCode(Http::STATUS_CODE statusCode)
{
    _statusCode = statusCode;
}

/**
 * @brief Sets the complete HTTP response including headers and body.
 */
void Response::setResponse()
{
    _response = getHeadersStr() + _body;
}

void Response::setCGIMatch() {
    std::string uri = _request.uri();
    std::string extension = CGIMatch().findExtension(uri);
    if (!extension.empty()) {
        for (StrStrMap::const_iterator it = _locationMatch->getCgiPathsBegin();
             it != _locationMatch->getCgiPathsEnd(); ++it) {
            if (it->first == extension) {
                _cgiMatch = CGIMatch(uri, it->second);
                Utils::log("CCGIMatch:", Utils::LOG_INFO);
                std::cout << _cgiMatch << std::endl;
                return;
            }
        }
    }
    Utils::log("No CGI match found for URI", Utils::LOG_INFO);
}
