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

void Response::handleFileList()
{
	std::string directory = "public/uploads"; // TODO: Get this from config

	try
	{
		std::vector<std::string> files = Directory::listFiles(directory);

		std::ostringstream json;
		json << "[";
		for (size_t i = 0; i < files.size(); ++i)
		{
			json << "\"" << files[i] << "\"";
			if (i < files.size() - 1)
			{
				json << ",";
			}
		}
		json << "]";

		_statusCode = Http::SC_OK;
		_body = json.str();
		_headers["Content-Type"] = "application/json";
	}
	catch (ExceptionMaker &e)
	{
		e.log();
		_statusCode = Http::SC_INTERNAL_SERVER_ERROR;
		_body = "{\"error\":\"Failed to list files in directory.\"}";
		_headers["Content-Type"] = "application/json";
	}
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
    else if (_request.method() == Http::M_POST)
        handlePOSTMethod();
    else if (_request.method() == Http::M_DELETE)
        handleDELETEMethod();
}

/**
 * @brief Deletes a specified file, handling errors and permissions.
 */
void Response::handleDELETEMethod()
{
	if (_request.uri().find("/delete") != 0)
		return setStatusAndReadResource(Http::SC_NOT_FOUND);

	std::string fileName = extractFileNameFromQuery(_request.uri());
	if (fileName.empty())
		return setStatusAndReadResource(Http::SC_BAD_REQUEST);

	std::string uploads_directory = "public/uploads"; // TODO: Get this from config
	std::string filePath = Utils::concatenatePaths(uploads_directory, fileName);

	if (Directory::isDirectory(filePath))
		return setStatusAndReadResource(Http::SC_FORBIDDEN);
	if (Utils::resourceExists(filePath))
	{
		if (remove(filePath.c_str()) != 0)
		{
			Utils::log("Error deleting file", Utils::LOG_ERROR);
			setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
		}
		else
		{
			_statusCode = Http::SC_NO_CONTENT;
		}
	}
	else
	{
		setStatusAndReadResource(Http::SC_NOT_FOUND);
	}
}

// TODO: the file content parsing should be called by the request parsing logic, not the response as it's being done now
void Response::handlePOSTMethod()
{
	if (_request.header("content-type").find("multipart/form-data") == std::string::npos)
		return setStatusAndReadResource(Http::SC_BAD_REQUEST);

	std::string fileName;
	std::string fileContent;
	if (!_request.parseFileContent(fileName, fileContent))
	{
		Utils::log("Error parsing file content", Utils::LOG_ERROR);
		return setStatusAndReadResource(Http::SC_BAD_REQUEST);
	}

	if (fileName.empty() || fileContent.empty())
	{
		Utils::log("Error parsing file content", Utils::LOG_ERROR);
		return setStatusAndReadResource(Http::SC_BAD_REQUEST);
	}

	std::string uploads_directory = "public/uploads"; // TODO: Get this from config
	std::string filePath = Utils::concatenatePaths(uploads_directory, fileName);

	try
	{
		std::ofstream outFile(filePath.c_str(), std::ios::binary);
		if (!outFile)
			throw ExceptionMaker("Error opening file for writing: " + filePath);

		outFile.write(fileContent.data(), fileContent.size());
		if (!outFile)
			throw ExceptionMaker("Error writing to file: " + filePath);

		_statusCode = Http::SC_CREATED;
		setHeader("Location", filePath);
		_body = "File uploaded successfully";
	}
	catch (ExceptionMaker &e)
	{
		e.log();
		return setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
	}
}

/**
 * @brief Handles GET requests by serving files, directories, or JSON file lists.
 *
 * - For "/files" URI, it invokes `handleFileList` to return a JSON list of files.
 * - For other URIs, it serves the requested file or directory listing, based on the existence and type of the resource.
 */
void Response::handleGETMethod()
{
	std::string root = _matchedLocation->getRootDir();
	bool autoindex = static_cast<LocationStatic*>(_matchedLocation)->getAutoIndex();

	if (_request.uri() == "/files")
	{
		handleFileList();
		return;
	}

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

void Response::handleCGI()
{
    std::string cgiPath = _matchedLocation->getRootDir() + _request.uri();
    std::string scriptPath = cgiPath;

    std::cout << "CGI Path: " << cgiPath << std::endl;
    std::cout << "Script Path: " << scriptPath << std::endl;

    setenv("REQUEST_METHOD", Http::methodToString(_request.method()).c_str(), 1);
    setenv("PATH_INFO", scriptPath.c_str(), 1);

    if (_request.method() == Http::M_POST)
    {
        setenv("CONTENT_TYPE", _request.header("Content-Type").c_str(), 1);
        setenv("CONTENT_LENGTH", _request.header("Content-Length").c_str(), 1);
    }

    int pipefd[2]; 
    if (pipe(pipefd) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process: handle CGI execution
        close(pipefd[1]); // Close write-end of the pipe
        
        // Redirect stdin from the pipe if POST method
        if (_request.method() == Http::M_POST) {
            if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                std::cerr << "dup2 failed for stdin" << std::endl;
                exit(1);
            }
        }
        
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[0]);

        execl(cgiPath.c_str(), cgiPath.c_str(), NULL);
        std::cerr << "execl failed: " << strerror(errno) << std::endl;
        exit(1);
    } 
    else if (pid > 0) {
        // Parent process
        close(pipefd[0]); // Close read-end of the pipe
        
        // Write POST data to the child process via pipe
        if (_request.method() == Http::M_POST) {
            write(pipefd[1], _request.body().data(), _request.body().size());
        }
        close(pipefd[1]); // Close write-end after writing

        char buffer[4096];
        ssize_t bytesRead;
        std::string output;

        fd_set readfds;
        struct timeval tv;
        tv.tv_sec = 30;  // 30 second timeout
        tv.tv_usec = 0;

        while (true) {
            FD_ZERO(&readfds);
            FD_SET(pipefd[0], &readfds);

            int ready = select(pipefd[0] + 1, &readfds, NULL, NULL, &tv);
            if (ready == -1) {
                std::cerr << "select() error" << std::endl;
                break;
            } else if (ready == 0) {
                std::cerr << "CGI script timeout" << std::endl;
                break;
            }

            bytesRead = read(pipefd[0], buffer, sizeof(buffer));
            if (bytesRead <= 0) break;
            output.append(buffer, bytesRead);
        }

        close(pipefd[0]);
        int status;
        waitpid(pid, &status, 0);

        _body = output;
        std::cout << "CGI Output: " << _body << std::endl;
    } 
    else {
        std::cerr << "Fork failed in handleCGI: " << strerror(errno) << std::endl;
    }

    setHeader("Content-Type", "text/html");
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

/**
 * @brief Sets the complete HTTP response including headers and body.
 */
void Response::setResponse()
{
    _response = getHeadersStr() + _body;
}