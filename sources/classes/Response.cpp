#include "../../includes/classes/Response.hpp"

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
void Response::setMatchedLocation()
{
    ServerLocation *bestMatch = NULL;
    std::string longestMatch;

    for (size_t j = 0; j < this->_serverConfigs.getLocationBlocksSize(); ++j)
    {
        ServerLocation *location = this->_serverConfigs.getLocationFromIndex(j);
        std::string locationPath = location->getLocation();
        if (_request.uri().find(locationPath) == 0 && locationPath.size() > longestMatch.size())
        {
            longestMatch = locationPath;
            bestMatch = location;
        }
    }

    if (bestMatch == NULL)
        bestMatch = this->_defaultLocation;

    _locationMatch = bestMatch;
	LOG("Matched location:", Utils::LOG_INFO);
	std::cout << *_locationMatch << std::endl;
}

/**************************************************************************/

/**********************  CONSTRUCTORS / DESTRUCTORS ***********************/

Response::~Response() 
{
	delete this->_defaultLocation;
}

Response::Response(Request const &request, ServerConfig const &configs)
    : _statusCode(Http::SC_OK),
      _headers(),
      _body(),
      _response(),
      _request(request),
      _serverConfigs(configs),
      _defaultLocation(new ServerLocation)
{
	if (this->_request.flag() != _EMPTY)
    {
		try
		{
	    	setMatchedLocation();
		}
		catch (ExceptionMaker &e)
		{
		    e.log();
		    setStatusAndReadResource(Http::SC_NOT_FOUND);
		}

		if (_request.flag() == _400)
		    setStatusAndReadResource(Http::SC_BAD_REQUEST);
		else if (_request.method() == Http::M_UNHANDLED)
		    setStatusAndReadResource(Http::SC_NOT_IMPLEMENTED);
		else if (isRedirection())
		    handleRedirection();
		else
		    dispatchMethod();

		setCommonHeaders();
	    setResponse();
	}
    std::cout << "Response headers:" << std::endl;
    std::cout << this->getHeadersStr() << std::endl;
}

/**************************************************************************/

/********************************  MEMBERS  *******************************/

/**
 * @brief Dispatches the request method. Calls the relevant method handler.
 */
void Response::dispatchMethod()
{
    if (!_locationMatch->methodIsAllowed(_request.method()))
        handleMethodNotAllowed();
	else if (setCGIMatch(), !_cgiMatch.getBinary().empty())
		handleCGI();
    else if (_request.method() == Http::M_GET)
    	handleGETMethod();
    else if (_request.method() == Http::M_POST)
        handlePOSTMethod();
    else if (_request.method() == Http::M_DELETE)
        handleDELETEMethod();
}

/**
 * @brief Sets status, error page and headers for the Method Not Allowed error.
 */
void Response::handleMethodNotAllowed()
{
    _statusCode = Http::SC_METHOD_NOT_ALLOWED;
    std::string allowedMethodsStr;
    for (size_t i = 0; i < _locationMatch->getMethodsAllowedSize(); ++i)
        allowedMethodsStr += Http::methodToString(_locationMatch->getMethodByIndex(i)) + (i < _locationMatch->getMethodsAllowedSize() - 1 ? ", " : "");
    setHeader("Allow", allowedMethodsStr);
    readResource(_locationMatch->getErrPagePath(_statusCode));
}

/**
 * @brief Handles the CGI process. Sets up the environment, forks and manages child and parent processes.
 */
void Response::handleCGI()
{
    std::string uri = _request.uri();
    std::string cgiPath = Utils::concatenatePaths(_locationMatch->getRootDir().c_str(), _cgiMatch.getBasePath().c_str(), _cgiMatch.getScriptName().c_str(), NULL);

    if (access(cgiPath.c_str(), F_OK) != 0)
        return setStatusAndReadResource(Http::SC_NOT_FOUND);
    if (access(cgiPath.c_str(), X_OK) != 0)
        return setStatusAndReadResource(Http::SC_FORBIDDEN);

    try
    {
        CGIHandler cgiHandler(*this, cgiPath);
        pid_t pid = fork();
        if (pid == 0)
            cgiHandler.handleChildProcess();
        else if (pid > 0)
            cgiHandler.handleParentProcess(pid);
        else
            throw ExceptionMaker("Fork failed: " + std::string(strerror(errno)));
    }
    catch (ExceptionMaker &e)
    {
        e.log();
        setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
    }
}

/**
 * @brief  Serves the requested file, and index file or a directory listing.
 */
void Response::handleGETMethod(void) {
    std::string root = _locationMatch->getRootDir();
    std::string uriPath = _request.uri();
    std::string uri = (uriPath == "/") ? root : Utils::concatenatePaths(root.c_str(), uriPath.c_str(), NULL);

    if (uriPath != "/" && (!uriPath.empty() && uriPath[uriPath.size() - 1] == '/')) {
        _locationMatch->getAutoIndex() ? listDirectory(uri) : setStatusAndReadResource(Http::SC_FORBIDDEN);
        return;
    }

    if (Directory::isDirectory(uri)) {
        for (size_t i = 0; i < _locationMatch->getIndexVectorSize(); i++) {
            std::string indexFile = Utils::concatenatePaths(uri.c_str(), _locationMatch->getIndexFileName(i).c_str(), NULL);
            if (access(indexFile.c_str(), F_OK) == 0) {
                this->readResource(indexFile);
                return;
            }
        }
        _locationMatch->getAutoIndex() ? listDirectory(uri) : setStatusAndReadResource(Http::SC_FORBIDDEN);
    } else if (access(uri.c_str(), F_OK) == 0) {
        this->readResource(uri);
    } else {
        setStatusAndReadResource(Http::SC_NOT_FOUND);
    }
}

//  IMPLEMENT THESE!!!
//  TEST WITH NGINX FIRST
void    Response::handlePOSTMethod(void)
{

}

void    Response::handleDELETEMethod(void)
{
    std::string    root = _locationMatch->getRootDir();
    std::string    uriPath = _request.uri().substr(_locationMatch->getLocation().size(), _request.uri().npos);
    std::string    resource = root + uriPath;

    if (access(resource.c_str(), F_OK) != 0)
        setStatusAndReadResource(Http::SC_NOT_FOUND);
    else if (Directory::isDirectory(resource) )
        setStatusAndReadResource(Http::SC_CONFLICT);
    else if (std::remove(resource.c_str() ) != 0)
        setStatusAndReadResource(Http::SC_FORBIDDEN);
    else
        setStatusAndReadResource(Http::SC_NO_CONTENT);
}

/**
 * @brief Generates a listing for the requested directory.
 */
void	Response::listDirectory(const std::string &path)
{
	std::ostringstream			webPage;
	std::map<std::string, std::set<std::string> >	fileList;

	struct stat										fileStat;
	struct tm										*tm;
	char											date[10];


	fileList = Directory::listFilesFull(path);

	webPage << "<!DOCTYPE html>\n";
	webPage << "<head>\n";
	webPage << "<title>Index of " << this->_request.uri() << "</title>\n";
	webPage << "</head>\n";
	webPage << "<body>\n";
	webPage << "<h1>Index of " << this->_request.uri() << "</h1><hr><pre><a href=\"../\">../</a>\n";

	for (std::set<std::string>::iterator itt = fileList["FOLDERS"].begin(); itt != fileList["FOLDERS"].end(); itt++)
	{
		std::string	folderPath = path + "/" + *itt;
		

		stat(folderPath.c_str(), &fileStat);
		tm = std::localtime(&fileStat.st_mtim.tv_sec);
		std::strftime(date, 11, "%d-%b-%y", tm);

		webPage << "<a href=\"" << this->_request.uri() << "/" << *itt << "\">" << *itt << "/</a>" << std::setw(75 - (*itt).size() - 1 ) << date << std::setw(20) << "-"  << "\n";		
	}

	for (std::set<std::string>::iterator itt = fileList["FILES"].begin(); itt != fileList["FILES"].end(); itt++)
	{
		std::string	filePath = path + "/" + *itt;


		stat(filePath.c_str(), &fileStat);
		tm = std::localtime(&fileStat.st_mtim.tv_sec);
		std::strftime(date, 11, "%d-%b-%y", tm);
		
		webPage << "<a href=\"" << this->_request.uri() << "/" << *itt << "\">" << *itt << "</a>" << std::setw(75 - (*itt).size() ) << date << std::setw(20) << fileStat.st_size  << "\n";
	}

	webPage << "</pre><hr></body>\n</html>\n";

	this->setHeader("Content-Type", "text/html");
	this->setBody(webPage.str() );
}

/**
 * @brief Reads the requested resource from the file system
 */
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
            LOG("Failed to load the internal server error page.", Utils::LOG_ERROR);
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

/**
 * @brief Looks for a redirection match in the location block.
 */
bool Response::isRedirection()
{
    std::string requestPath = _request.uri();
    std::string locationPath = _locationMatch->getLocation();

    if (requestPath.find(locationPath) == 0)
    {
        std::string relativePath = requestPath.substr(locationPath.size());

        StrStrMap::const_iterator it = _locationMatch->getRedirectionIttBegin();
        StrStrMap::const_iterator end = _locationMatch->getRedirectionIttEnd();

        for (; it != end; ++it)
        {
            if (relativePath == it->first)
            {
                _redirectionPath = locationPath + it->second;
                return true;
            }
        }
    }
    return false;
}

void Response::handleRedirection()
{
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

/**
 * @brief Matches the extension of the requested URI with the CGI extensions in the location block.
 */
void Response::setCGIMatch()
{
    std::string uri = _request.uri();
    std::string extension = CGIMatch().findExtension(uri);
    if (!extension.empty())
    {
        for (StrArr::const_iterator it = _locationMatch->getCgiExtensionsBegin();
             it != _locationMatch->getCgiExtensionsEnd(); ++it)
        {
            if (*it == extension)
            {
                _cgiMatch = CGIMatch(uri, *it);
                LOG("CCGIMatch:", Utils::LOG_INFO);
                std::cout << _cgiMatch << std::endl;
                return;
            }
        }
    }
    LOG("No CGI match found for URI", Utils::LOG_INFO);
}
