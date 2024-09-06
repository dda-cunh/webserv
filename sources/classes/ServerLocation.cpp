#include "../../includes/classes/ServerLocation.hpp"


ServerLocation::ServerLocation(void)
{
	this->_location = "/";
	this->_rootDir = "./test_files";
	this->_indexFile = "index.html";
	this->_maxBodySize = 1000000;
	this->_errorPages[404] = "./test_files/error_pages/404.html";
	this->_errorPages[405] = "./test_files/error_pages/405.html";
	this->_errorPages[500] = "./test_files/error_pages/500.html";
	//	leave redirections empty for now
	this->_methodsAllowed.push_back(Http::M_GET);
	this->_methodsAllowed.push_back(Http::M_POST);
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
	return (this->_errorPages.at(status));
}

std::string	ServerLocation::getRedirection(int status) const
{
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

LocationStatic::LocationStatic(void)
{
	this->_autoIndex = false;
}

LocationStatic::LocationStatic(const LocationStatic &locationStatic): ServerLocation()
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
