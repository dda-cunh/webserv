#include "../../includes/classes/Response.hpp"

/**************************************************************************/

/****************************  CANNONICAL FORM  ****************************/

Response::Response()
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
	std::string directory = "test_files/uploads"; // TODO: Get this from config
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

/**************************************************************************/

/*****************************  CONSTRUCTORS  *****************************/

Response::Response(Request const &request)
	: _statusCode(Http::SC_OK),
	  _headers(),
	  _body(),
	  _response(),
	  _request(request),
	  _error_pages(dummyGetErrorPages()),
	  _redirections(dummyGetRedirections())
{
	if (_request.flag() == _400)
		setStatusAndReadResource(Http::SC_BAD_REQUEST);
	else if (isRedirection())
		;
	else
		dispatchMethod();
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

	std::string uploads_directory = "test_files/uploads"; // TODO: Get this from config
	std::string filePath = uploads_directory + "/" + fileName;

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
			_body = "File" + fileName + " deleted successfully";
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
	std::string filePath = "test_files/uploads/" + fileName; // TODO: Get this from config
	std::ofstream outFile(filePath.c_str(), std::ios::binary);
	if (!outFile)
	{
		Utils::log("Error opening file for writing", Utils::LOG_ERROR);
		return setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
	}
	outFile.write(fileContent.data(), fileContent.size());
	outFile.close();
	if (!outFile)
	{
		Utils::log("Error writing to file", Utils::LOG_ERROR);
		return setStatusAndReadResource(Http::SC_INTERNAL_SERVER_ERROR);
	}
	_statusCode = Http::SC_CREATED;
	setHeader("Location", filePath);
	_body = "File uploaded successfully"; // TODO: Return HTML in the body
}

/**
 * @brief Handles GET requests by serving files, directories, or JSON file lists.
 *
 * - For "/files" URI, it invokes `handleFileList` to return a JSON list of files.
 * - For other URIs, it serves the requested file or directory listing, based on the existence and type of the resource.
 */
void Response::handleGETMethod()
{
	std::string root = "test_files/www"; // TODO: get this from config
	bool autoindex = true;				 // TODO: get this from config

	if (_request.uri() == "/files")
	{
		handleFileList();
		return;
	}

	std::string uri = (_request.uri() == "/") ? root : root + "/" + _request.uri();

	if (Directory::isDirectory(uri))
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

void Response::readResource(std::string uri)
{
	std::ifstream file(uri.c_str(), std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	setHeader("Content-Type", getResourceContentType(uri));
	_body = content;
}

void Response::setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri)
{
	_statusCode = statusCode;
	if (!uri.empty())
		readResource(uri);
	else
		readResource(_error_pages[_statusCode]);
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

void Response::setErrorPages()
{
	_error_pages = dummyGetErrorPages();
}
