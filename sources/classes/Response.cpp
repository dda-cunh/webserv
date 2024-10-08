#include "../../includes/classes/Response.hpp"

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

// TODO: Get this from config and remove function
std::vector<Http::METHOD> getAllowedMethods()
{
	std::vector<Http::METHOD> allowedMethods;
	allowedMethods.push_back(Http::M_GET);
	allowedMethods.push_back(Http::M_DELETE);
	allowedMethods.push_back(Http::M_POST);
	// Add more methods or retrieve from config as needed
	return allowedMethods;
}

// TODO: Get this from config and remove function
StrStrMap dummyGetRedirections()
{
	StrStrMap redirections;

	redirections["/old-path.html"] = "/new-path.html";
	redirections["/old-file"] = "/new-file";

	return redirections;
}

std::string Response::getResponseWithoutBody() // TODO: Debug function, to be removed
{
	std::string responseWithoutBody;
	responseWithoutBody += "HTTP/1.1 " + Utils::intToString(_statusCode) + " " + Http::sToReasonPhrase(_statusCode) + CRLF;
	for (StrStrMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		responseWithoutBody += it->first + ": " + it->second + CRLF;
	responseWithoutBody += CRLF;
	return responseWithoutBody;
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
 * @brief 
 *
 * This function will match the response to the right location/route
 */
void Response::setLocation() {
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

    // Assuming _matchedLocation is of type LocationStatic
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
    setLocation();

    if (_request.flag() == _400) {
        setStatusAndReadResource(Http::SC_BAD_REQUEST);
    } else if (_request.method() == Http::M_UNHANDLED) {
        setStatusAndReadResource(Http::SC_NOT_IMPLEMENTED);
    } else if (isRedirection()) {
        // Handle redirection
    } else {
        dispatchMethod();
    }

    setCommonHeaders();
    setResponse();
    Utils::log("Response:", Utils::LOG_INFO);
    Utils::log(getResponseWithoutBody(), Utils::LOG_INFO);
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
	std::vector<Http::METHOD> allowedMethods = getAllowedMethods(); // TODO: Get this from config
	if (std::find(allowedMethods.begin(), allowedMethods.end(), _request.method()) == allowedMethods.end())
		handleMethodNotAllowed();
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
		_body = "File uploaded successfully"; // TODO: Return HTML in the body
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
	std::string root = "public/"; // TODO: get this from config
	bool autoindex = true;		  // TODO: get this from config

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
		uri = _statusCode == Http::SC_OK ? result.path : _error_pages[_statusCode];
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
	std::vector<Http::METHOD> allowedMethods = getAllowedMethods(); // TODO: Get this from config
	std::string allowedMethodsStr;
	for (size_t i = 0; i < allowedMethods.size(); ++i)
		allowedMethodsStr += Http::methodToString(allowedMethods[i]) + (i < allowedMethods.size() - 1 ? ", " : "");
	setHeader("Allow", allowedMethodsStr);
	readResource(_error_pages[_statusCode]);
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
		readResource(_error_pages[_statusCode], true);
}

bool Response::isRedirection()
{
	std::map<std::string, std::string>::iterator it = _redirections.find(_request.uri());
	if (it != _redirections.end())
	{
		_statusCode = Http::SC_FOUND;
		setHeader("Location", it->second);
		return true;
	}
	return false;
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
