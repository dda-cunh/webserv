#include "../../includes/classes/ServerLocation.hpp"


ServerLocation::ServerLocation(void)
{
	this->_location = DEFAULT_LOCATION;
	this->_rootDir = DEFAULT_ROOT;
	this->_indexFiles.push_back(DEFAULT_INDEX);
	this->_maxBodySize = DEFAULT_MAX_BODY_SIZE;
	this->_errorPages[404] = DEFAULT_404;
	this->_errorPages[405] = DEFAULT_405;
	this->_errorPages[500] = DEFAULT_500;
	
	this->_methodsAllowed.push_back(Http::M_GET);
	this->_methodsAllowed.push_back(Http::M_POST);
}

ServerLocation::ServerLocation(std::vector<std::string> strLocationBlock)
{
	this->_location = this->_setLocation(strLocationBlock.at(0) );
	this->_rootDir = this->_setRootDir(strLocationBlock);
	this->_setIndexFiles(strLocationBlock, this->_indexFiles);
	this->_maxBodySize = this->_setMaxBodySize(strLocationBlock);

	this->_setErrorPages(strLocationBlock, this->_errorPages);
	this->_setRedirections(strLocationBlock, this->_redirections);
	this->_setAllowedMethods(strLocationBlock, this->_methodsAllowed);
	this->_uploadPath = this->_setUploadStore(strLocationBlock);
}

ServerLocation::ServerLocation(const ServerLocation &serverLocation)
{
	if (this != &serverLocation)
		*this = serverLocation;
}

ServerLocation::~ServerLocation(void)
{
	return ;
}

ServerLocation	&ServerLocation::operator=(const ServerLocation &serverLocation)
{
	size_t	indexVectorSize;

	this->_location = serverLocation.getLocation();
	this->_rootDir = serverLocation.getRootDir();

	indexVectorSize = serverLocation.getIndexVectorSize();
	for (size_t i = 0; i < indexVectorSize; i++)
		this->_indexFiles.push_back(serverLocation.getIndexFileName(i) );
	
	this->_maxBodySize = serverLocation.getMaxBodySize();
	
	for (IntStrMap::const_iterator itt = serverLocation.getErrPageIttBegin(); itt != serverLocation.getErrPageIttEnd(); itt++)
		this->_errorPages[itt->first] = itt->second;

	for (StrStrMap::const_iterator itt = serverLocation.getRedirectionIttBegin(); itt != serverLocation.getRedirectionIttEnd(); itt++)
		this->_redirections[itt->first] = itt->second;

	for (size_t i = 0; i < serverLocation.getMethodsAllowedSize(); i++)
		this->_methodsAllowed.push_back(serverLocation.getMethodByIndex(i) );

  this->_uploadPath = serverLocation.getUploadPath();

	return (*this);
}

std::string	ServerLocation::getLocation(void) const
{
	return (this->_location);
}

std::string	ServerLocation::getRootDir(void) const
{
	return (this->_rootDir);
}

std::string	ServerLocation::getIndexFileName(size_t i) const
{
	if (i < this->_indexFiles.size() )
		return (this->_indexFiles.at(i) );
	else
		throw (ExceptionMaker("Index for index files out of range") );
}

uint32_t	ServerLocation::getMaxBodySize(void) const
{
	return (this->_maxBodySize);
}

IntStrMap::const_iterator	ServerLocation::getErrPageIttBegin(void) const
{
	return (this->_errorPages.begin());
}

IntStrMap::const_iterator	ServerLocation::getErrPageIttEnd(void) const
{
	return (this->_errorPages.end());
}

StrStrMap::const_iterator	ServerLocation::getRedirectionIttBegin(void) const
{
	return (this->_redirections.begin());
}

StrStrMap::const_iterator	ServerLocation::getRedirectionIttEnd(void) const
{
	return (this->_redirections.end());
}

Http::METHOD	ServerLocation::getMethodByIndex(size_t i) const
{
	if (i >= this->_methodsAllowed.size() )
		throw (ExceptionMaker("Allowed methods index is out of bounds") );
	return (this->_methodsAllowed.at(i));
}

size_t	ServerLocation::getMethodsAllowedSize(void) const
{
	return (this->_methodsAllowed.size());
}

std::string	ServerLocation::getErrPagePath(int status) const
{
	if (this->_errorPages.find(status) == this->_errorPages.end() )
		throw (ExceptionMaker("Invalid edge status code") );
	return (this->_errorPages.at(status));
}

std::string	ServerLocation::getRedirection(std::string url) const
{
	if (this->_redirections.find(url) == this->_redirections.end() )
		throw (ExceptionMaker("No redirection found for URL provided") );
	return (this->_redirections.at(url));
}

bool	ServerLocation::methodIsAllowed(Http::METHOD method) const
{
	size_t	vectorSize;

	vectorSize = this->_methodsAllowed.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (this->_methodsAllowed.at(i) == method)
			return (true);
	}
	return (false);
}

std::string	ServerLocation::getUploadPath(void) const
{
	return (this->_uploadPath);
}

size_t	ServerLocation::getIndexVectorSize(void) const
{
	return (this->_indexFiles.size() );
}


std::string	ServerLocation::_setLocation(std::string locationLine)
{
	locationLine = SyntaxChecker::strParseLine(locationLine);

	if (!locationLine.empty() || Utils::sWordCount(locationLine) != 1)
		return (locationLine);
	else
		throw (ExceptionMaker("Invalid number of arguments in \"location\" directive") );
}

std::string	ServerLocation::_setRootDir(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("root") == 0)
		{
				line = SyntaxChecker::strParseLine(line);
				if (Utils::sWordCount(line) > 1)
					throw (ExceptionMaker("Invalid number of arguments in \"root\" directive") );
				return (line);
		}
	}

	if (ConfigParser::defaultRoot.empty() )
		return (DEFAULT_ROOT);
	else
		return (ConfigParser::defaultRoot);
}

void	ServerLocation::_setIndexFiles(std::vector<std::string> strLocationBlock, std::vector<std::string> &indexFiles)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("index") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			ConfigParser::strToVecSplit(line, indexFiles);
		}
	}
	
	if (indexFiles.empty() )
	{
		if (ConfigParser::defaultIndex.empty() )
			indexFiles.push_back(DEFAULT_INDEX);
		else
		{
			for (size_t i = 0; i < ConfigParser::defaultIndex.size(); i++)
				indexFiles.push_back(ConfigParser::defaultIndex.at(i) );
		}
	}
}

uint32_t	ServerLocation::_setMaxBodySize(std::vector<std::string> strLocationBlock)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	uint32_t			unMaxBodySize;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("client_max_body_size") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strStream << line;
			strStream >> unMaxBodySize;
			return (unMaxBodySize);
		}
	}

	return (ConfigParser::defaultMaxBodySize);	
}

void	ServerLocation::_setErrorPages(std::vector<std::string> strLocationBlock, IntStrMap &errorPages)
{
	size_t				vectorSize;
	std::string			line;
	int					errCode;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("error_page") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			errCode = std::atoi(line.substr(0, line.find_first_of(" \t") ).c_str() );
			if (errorPages.find(errCode) != errorPages.end() )
				throw (ExceptionMaker("Multiple settings for the same error page in location context") );
			errorPages[errCode] = line.substr(line.find_last_of(" \t") + 1);
		}
	}


	if (errorPages.find(400) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(400) == ConfigParser::defaultErrorPages.end() )
			errorPages[400] = DEFAULT_400;
		else
			errorPages[400] = ConfigParser::defaultErrorPages[400];
	}

	if (errorPages.find(403) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(403) == ConfigParser::defaultErrorPages.end() )
			errorPages[403] = DEFAULT_403;
		else
			errorPages[403] = ConfigParser::defaultErrorPages[403];
	}

	if (errorPages.find(404) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(404) == ConfigParser::defaultErrorPages.end() )
			errorPages[404] = DEFAULT_404;
		else
			errorPages[404] = ConfigParser::defaultErrorPages[404];
	}

	if (errorPages.find(405) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(405) == ConfigParser::defaultErrorPages.end() )
			errorPages[405] = DEFAULT_405;
		else
			errorPages[405] = ConfigParser::defaultErrorPages[405];
	}

	if (errorPages.find(500) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(500) == ConfigParser::defaultErrorPages.end() )
			errorPages[500] = DEFAULT_500;
		else
			errorPages[500] = ConfigParser::defaultErrorPages[500];
	}

	if (errorPages.find(501) == errorPages.end() )
	{
		if (ConfigParser::defaultErrorPages.find(501) == ConfigParser::defaultErrorPages.end() )
			errorPages[501] = DEFAULT_501;
		else
			errorPages[501] = ConfigParser::defaultErrorPages[501];
	}
}

void	ServerLocation::_setRedirections(std::vector<std::string> strLocationBlock, StrStrMap &redirections)
{
	size_t		vectorSize;
	std::string	line;
	std::string	uri;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("rewrite") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			uri = line.substr(0, line.find_first_of(" \t") );
			if (redirections.find(uri) == redirections.end() )
				redirections[uri] = Utils::sTrim(line.substr(line.find_last_of(" \t") ) );
			else
				throw (ExceptionMaker("Duplicate redirection provided in \"rewrite\" directives") );
		}
	}

	for (StrStrMap::iterator itt = ConfigParser::defaultRedirections.begin(); itt != ConfigParser::defaultRedirections.end(); itt++)
	{
		if (redirections.find(itt->first) == redirections.end() )
			redirections[itt->first] = itt->second;
	}
}

void	ServerLocation::_setAllowedMethods(std::vector<std::string> strLocationBlock, std::vector<Http::METHOD> &methodsAllowed)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	std::string			method;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("allow_methods") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strStream << line;
			while (strStream >> method)
			{
				if (method == "GET")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_GET) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );
					methodsAllowed.push_back(Http::M_GET);
				}
				else if (method == "POST")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_POST) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );
					methodsAllowed.push_back(Http::M_POST);
				}
				else if (method == "DELETE")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_DELETE) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );
					methodsAllowed.push_back(Http::M_DELETE);
				}
				else
					throw (ExceptionMaker("Invalid argument in \"allow_methods\" directive") );
			}
		}
	}

	if (methodsAllowed.empty() )
	{
		if (ConfigParser::defaultMethodsAllowed.empty() )
			methodsAllowed.push_back(Http::M_GET);
		else
		{
			for (size_t i = 0; i < ConfigParser::defaultMethodsAllowed.size(); i++)
			{
				if (ConfigParser::defaultMethodsAllowed.at(i) == "GET")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_GET) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );
					methodsAllowed.push_back(Http::M_GET);
				}
				else if (ConfigParser::defaultMethodsAllowed.at(i) == "POST")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_POST) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );
					methodsAllowed.push_back(Http::M_POST);
				}
				else if (ConfigParser::defaultMethodsAllowed.at(i) == "DELETE")
				{
					if (std::find(methodsAllowed.begin(), methodsAllowed.end(), Http::M_DELETE) != methodsAllowed.end() )
						throw (ExceptionMaker("Duplicate method found in \"allow_methods\" directive") );					
					methodsAllowed.push_back(Http::M_DELETE);
				}
				else
					throw (ExceptionMaker("Invalid argument in \"allow_methods\" directive in server context") );
			}
		}
	}
}

std::string	ServerLocation::_setUploadStore(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("upload_store") == 0)
			return (SyntaxChecker::strParseLine(line) );
	}

	return (ConfigParser::defaultUploadPath);
}



//	DERIVED CLASSES

//		STATIC SITE
LocationStatic::LocationStatic(void)
{
	this->_autoIndex = false;
}


LocationStatic::LocationStatic(std::vector<std::string> strLocationBlock): ServerLocation(strLocationBlock)
{
	this->_autoIndex = 	this->_setAutoIndex(strLocationBlock);
}


LocationStatic::LocationStatic(const LocationStatic &locationStatic): ServerLocation(locationStatic)
{
	if (this != &locationStatic)
		*this = locationStatic;

}

LocationStatic::~LocationStatic(void)
{
	return ;
}

LocationStatic	&LocationStatic::operator=(const LocationStatic &locationStatic)
{
	this->_autoIndex = locationStatic.getAutoIndex();

	return (*this);
}

bool	LocationStatic::getAutoIndex(void) const
{
	return (this->_autoIndex);
}


bool	LocationStatic::_setAutoIndex(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);	
		if (line.find("autoindex") == 0)
		{
			line = SyntaxChecker::strParseLine(line);

			if (line == "on")
				return (true);
			else if (line == "off")
				return (false);
			else
				throw (ExceptionMaker("Invalid argument in \"autoindex\" directive") );
		}
	}

	return (ConfigParser::defaultAutoIndex);	
}


LocationCGI::LocationCGI(void)
{
}

LocationCGI::LocationCGI(std::vector<std::string> strLocationBlock): ServerLocation(strLocationBlock)
{
	this->_setCgiPaths(strLocationBlock);	
}

LocationCGI::LocationCGI(const LocationCGI &locationCGI): ServerLocation(locationCGI)
{
	if (this != &locationCGI)
		*this = locationCGI;
}

LocationCGI	&LocationCGI::operator=(const LocationCGI &locationCGI)
{
	for (StrStrMap::const_iterator itt = locationCGI.getCgiPathsBegin(); itt != locationCGI.getCgiPathsEnd(); itt++)
		this->_cgiPaths[itt->first] = itt->second;

	return (*this);
}

LocationCGI::~LocationCGI(void)
{
	return ;
}


std::string	LocationCGI::getCgiPath(std::string ext) const
{
	return (this->_cgiPaths.at(ext) );
}


StrStrMap::const_iterator	LocationCGI::getCgiPathsBegin(void) const
{
	return (this->_cgiPaths.begin() );
}

StrStrMap::const_iterator	LocationCGI::getCgiPathsEnd(void) const
{
	return (this->_cgiPaths.end() );
}



void	LocationCGI::_setCgiPaths(std::vector<std::string> strLocationBlock)
{
	size_t		vectorSize;
	std::string	line;
	std::string	ext;

	vectorSize = strLocationBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strLocationBlock.at(i);
		if (line.find("cgi_path") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			ext = line.substr(0, line.find_first_of(" \t") );
			if (this->_cgiPaths.find(ext) == this->_cgiPaths.end() )
				this->_cgiPaths[ext] = line.substr(line.find_last_of(" \t") );
			else
				throw (ExceptionMaker("Duplicate extension provided in \"cgi_path\" directive") );
		}
	}
}

std::ostream 	&operator<<(std::ostream &out, const LocationStatic &locationStatic)
{
	size_t	indexVectorSize;

	out << "\tLocation: " << locationStatic.getLocation() << std::endl;
	out << "\tRoot: " << locationStatic.getRootDir() << std::endl;
  
	out << "\tIndex files:" << std::endl;
	indexVectorSize = locationStatic.getIndexVectorSize();
	for (size_t i = 0; i < indexVectorSize; i++)
		out << "\t\t" << locationStatic.getIndexFileName(i) << std::endl;

	out << "\tMax body size: " << locationStatic.getMaxBodySize() << std::endl;

	out << "\tError pages:" << std::endl;
	for (IntStrMap::const_iterator itt = locationStatic.getErrPageIttBegin(); itt != locationStatic.getErrPageIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tRedirections:" << std::endl;
	for (StrStrMap::const_iterator itt = locationStatic.getRedirectionIttBegin(); itt != locationStatic.getRedirectionIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tAllowed methods:" << std::endl;
	for (size_t i = 0; i < locationStatic.getMethodsAllowedSize(); i++)
		out << "\t\t" << locationStatic.getMethodByIndex(i) << std::endl;

	out << "\tUpload path: " << locationStatic.getUploadPath() << std::endl;

	out << "\tAutoindex: " << locationStatic.getAutoIndex() << std::endl;

	return (out);
}

std::ostream 	&operator<<(std::ostream &out, const LocationCGI &locationCGI)
{
	out << "\tLocation: " << locationCGI.getLocation() << std::endl;
	out << "\tRoot: " << locationCGI.getRootDir() << std::endl;

	out << "\tIndex files:" << std::endl;
	for (size_t i = 0; i < locationCGI.getIndexVectorSize(); i++)
		out << "\t\t" << locationCGI.getIndexFileName(i) << std::endl;

	out << "\tMax body size: " << locationCGI.getMaxBodySize() << std::endl;

	out << "\tError pages:" << std::endl;
	for (IntStrMap::const_iterator itt = locationCGI.getErrPageIttBegin(); itt != locationCGI.getErrPageIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tRedirections:" << std::endl;
	for (StrStrMap::const_iterator itt = locationCGI.getRedirectionIttBegin(); itt != locationCGI.getRedirectionIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "Allowed methods:" << std::endl;
	for (size_t i = 0; i < locationCGI.getMethodsAllowedSize(); i++)
		out << "\t\t" << locationCGI.getMethodByIndex(i) << std::endl;

	out << "\tUpload path: " << locationCGI.getUploadPath() << std::endl;

	out << "\tCGI:" << std::endl;
	for (StrStrMap::const_iterator itt = locationCGI.getCgiPathsBegin(); itt != locationCGI.getCgiPathsEnd(); itt++)
		out << "\t\t" << itt->first << " " << locationCGI.getCgiPath(itt->first) << std::endl;

	return (out);
}
