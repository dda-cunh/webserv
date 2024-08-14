#include "../../includes/classes/ServerLocation.hpp"

//	BASE CLASS

ServerLocation::ServerLocation(void)
{
	this->_location = "/";
	this->_rootDir = "./test_files/error_pages/"
	this->_indexFile = "index.html";	//	DONT FORGET TO CREATE THIS FILE
	this->_maxBodySize = 1000000;
	this->_errorPages[404] = "./test_files/error_pages/404.html";
	this->_errorPages[405] = "./test_files/error_pages/405.html";
	this->_errorPages[500] = "./test_files/error_pages/500.html";
	//	leave redirections empty for now
	this->_methodsAllowed.push_back(Http::M_GET);
	this->_methodsAllowed.push_back(Http::M_POST);
}

/*
ServerLocation::ServerLocation(std::vector<std::string> strLocationBlock)
{

}
*/

ServerLocation::ServerLocation(const ServerLocation &serverLocation)
{
	if (this != &serverLocation)
		*this = serverLocation;
}

ServerLocation	ServerLocation::&operator=(const ServerLocation &serverLocation)
{
	this->_location = serverLocation.getLocation();
	this->_rootDir = serverLocation.getRootDir();
	this->_indexFile = serverLocation.getIndexFilename();
	this->_maxBodySize = serverLocation.getMaxBodySize();
	
	for (IntStrMap::iterator itt = serverLocation.getErrPageIttBegin(); itt != serverLocation.getErrPageIttEnd(); itt++)
		this->_errorPages[itt->first] = itt->second;

	for (IntStrMap::iterator itt = serverLocation.getRedirectionIttBegin(); itt != serverLocation.getRedirectionIttEnd(); itt++)
		this->_redirections[itt->first] = itt->second;

	for (size_t i = 0; i < serverLocation.getMethodsAllowedSize(); i++)
		this->_methodsAllowed[i] = serverLocation.getMethodByIndex(i);

	return (*this);
}

std::string	ServerLocation::getLocation(void)
{
	return (this->_location);
}

std::string	ServerLocation::getRootDir(void)
{
	return (this->_rootDir);
}

std::string	ServerLocation::getIndexFilename(void)
{
	return (this->_indexFile);
}

uint32_t	ServerLocation::getMaxBodySize(void)
{
	return (this->_maxBodySize);
}

std::string	ServerLocation::getErrPagePath(int status)
{
	//	THIS FUNCTION MUST HANDLE EDGE CASES!!!!!
	return (this->_errorPages[status]);
}

std::string	ServerLocation::getRedirection(int status)
{
	//	THIS FUNCTION MUST HANDLE EDGE CASES!!!!!
	return (this->_redirections[status]);
}

bool	ServerLocation::methodIsAllowed(Http::METHOD method)
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


//	DERIVATED CLASSES

//		STATIC SITE
LocationStatic::LocationStatic(void)	//	INIT BASE CLASS TOO
{
	this->_autoIndex = false;
}

/*
LocationStatic::LocationStatic(std::vector<std::string> strLocationBlock)	//	init base class too
{

}
*/

LocationStatic::LocationStatic(const LocationStatic &locationStatic)
{
	if (this != &locationStatic)
		*this = locationStatic;

}

LocationStatic::~LocationStatic(void)
{
	return ;
}

LocationStatic	LocationStatic::&operator=(const LocationStatic &locationStatic)
{
	this->_autoIndex = locationStatic.getAutoIndex();

	return (*this);
}

bool	LocationStatic::getAutoIndex(void)
{
	return (this->_autoIndex);
}


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

std::ostream 	&operator<<(std::ostream &out, const LocationStatic &locationStatic)
{
	out << locationStatic.getLocation() << std::endl;
	out << locationStatic.getRootDir() << std::endl;
	out << locationStatic.getIndexFilename() << std::endl;
	out << locationStatic.getMaxBodySize() << std::endl;
	//	AND THEN, MAPS

	return (out);
}

std::ostream 	&operator<<(std::ostream &out, const LocationCGI &locationCGI)
{
	out << locationCGI.getLocation() << std::endl;
	out << locationCGI.getRootDir() << std::endl;
	out << locationCGI.getIndexFilename() << std::endl;
	out << locationCGI.getMaxBodySize() << std::endl;
	//	AND THEN, MAPS

	return (out);
}
