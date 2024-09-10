#include "../../includes/classes/ServerLocation.hpp"


//	BASE CLASS

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
	//	PARSE DIRECTIVES FROM VECTOR
	this->_location = ConfigParser::parseLocation(strLocationBlock);
	this->_rootDir = ConfigParser::parseRootDir(strLocationBlock);
	ConfigParser::parseIndexFiles(strLocationBlock, this->_indexFiles);
	this->_maxBodySize = ConfigParser::parseMaxBodySize(strLocationBlock);

	//	FIND ERROR PAGES DIRECTIVE AND PARSE ITS RESPECTIVE KEY/VALUE PAIRS
	//		INTO this->_errorPages
	//	SAME WITH REDIRECTIONS
	//	AND ALLOWED METHODS
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
	this->_location = serverLocation.getLocation();
	this->_rootDir = serverLocation.getRootDir();
	this->_indexFile = serverLocation.getIndexFilename();
	this->_maxBodySize = serverLocation.getMaxBodySize();
	
	for (IntStrMap::const_iterator itt = serverLocation.getErrPageIttBegin(); itt != serverLocation.getErrPageIttEnd(); itt++)
		this->_errorPages[itt->first] = itt->second;

	for (IntStrMap::const_iterator itt = serverLocation.getRedirectionIttBegin(); itt != serverLocation.getRedirectionIttEnd(); itt++)
		this->_redirections[itt->first] = itt->second;

	for (size_t i = 0; i < serverLocation.getMethodsAllowedSize(); i++)
		this->_methodsAllowed[i] = serverLocation.getMethodByIndex(i);

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

std::string	ServerLocation::getIndexFilename(void) const
{
	return (this->_indexFile);
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

IntStrMap::const_iterator	ServerLocation::getRedirectionIttBegin(void) const
{
	return (this->_redirections.begin());
}

IntStrMap::const_iterator	ServerLocation::getRedirectionIttEnd(void) const
{
	return (this->_redirections.end());
}

Http::METHOD	ServerLocation::getMethodByIndex(size_t i) const
{
	return (this->_methodsAllowed.at(i));
}

size_t	ServerLocation::getMethodsAllowedSize(void) const
{
	return (this->_methodsAllowed.size());
}

std::string	ServerLocation::getErrPagePath(int status) const
{
	//	THIS FUNCTION MUST HANDLE EDGE CASES!!!!!
	return (this->_errorPages.at(status));
}

std::string	ServerLocation::getRedirection(int status) const
{
	//	THIS FUNCTION MUST HANDLE EDGE CASES!!!!!
	return (this->_redirections.at(status));
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


//	DERIVED CLASSES

//		STATIC SITE
LocationStatic::LocationStatic(void)
{
	this->_autoIndex = false;
}


LocationStatic::LocationStatic(std::vector<std::string> strLocationBlock): serverLocation(strLocationBlock)
{
	this->_autoIndex = 	ConfigParser::parseAutoIndex(strLocationBlock);
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


/*
//		REVERSE PROXY (FOR FILE UPLOADS)
LocationRevProxy::LocationRevProxy(void)
{
	//	what will default proxy pass be?
	this->_uploadDirectory = "/upload";
}


LocationRevProxy::LocationRevProxy(std::vector<std::string> strLocationBlock)
{

}


LocationRevProxy::~LocationRevProxy(void)
{
	return ;
}

std::string	LocationRevProxy::getUploadDir(void)
{
	return (this->_uploadDirectory);
}


//		CGI

LocationCGI::LocationCGI(void)
{
	//	INIT WITH DEFAULT VALUES
}

LocationCGI::LocationCGI(std::vector<std::string> strLocationBlock)
{

}

LocationCGI::LocationCGI(const LocationCGI &locationCGI)
{
	if (this != &locationCGI)
		*this = locationCGI;
}

LocationCGI	LocationCGI::&operator=(const LocationCGI &locationCGI)
{
	//	COPY VALUES

	return (*this);
}

LocationCGI::~LocationCGI(void)
{
	return ;
}
*/

std::ostream 	&operator<<(std::ostream &out, const LocationStatic &locationStatic)
{
	out << "\tLocation: " << locationStatic.getLocation() << std::endl;
	out << "\tRoot: " << locationStatic.getRootDir() << std::endl;
	out << "\tIndex: " << locationStatic.getIndexFilename() << std::endl;
	out << "\tMax body size: " << locationStatic.getMaxBodySize() << std::endl;

	out << "\tError pages:" << std::endl;
	for (IntStrMap::const_iterator itt = locationStatic.getErrPageIttBegin(); itt != locationStatic.getErrPageIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tRedirections:" << std::endl;
	for (IntStrMap::const_iterator itt = locationStatic.getRedirectionIttBegin(); itt != locationStatic.getRedirectionIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tAllowed methods:" << std::endl;
	for (size_t i = 0; i < locationStatic.getMethodsAllowedSize(); i++)
		out << "\t\t" << locationStatic.getMethodByIndex(i) << std::endl;

	out << "\tAutoindex: " << locationStatic.getAutoIndex() << std::endl;

	return (out);
}

/*
std::ostream 	&operator<<(std::ostream &out, const LocationCGI &locationCGI)
{
	out << "\tLocation: " << locationCGI.getLocation() << std::endl;
	out << "\tRoot: " << locationCGI.getRootDir() << std::endl;
	out << "\tIndex: " << locationCGI.getIndexFilename() << std::endl;
	out << "\tMax body size: " << locationCGI.getMaxBodySize() << std::endl;

	out << "\tError pages:" << std::endl;
	for (IntStrMap::iterator itt = locationCGI.getErrPageIttBegin(); itt != locationCGI.getErrPageIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tRedirections:" << std::endl;
	for (IntStrMap::iterator itt = locationCGI.getRedirectionIttBegin(); itt != locationCGI.getRedirectionIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "Allowed methods:" << std::endl;
	for (size_t i = 0; i < locationCGI.getMethodsAllowedSize(); i++)
		out << "\t\t" << locationCGI.getMethodByIndex(i) << std::endl;



	return (out);
}
*/