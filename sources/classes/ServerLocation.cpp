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
	this->_location = ConfigParser::parseLocation(strLocationBlock.at(0) );
	this->_rootDir = ConfigParser::parseRootDir(strLocationBlock);
	ConfigParser::parseIndexFiles(strLocationBlock, this->_indexFiles);
	this->_maxBodySize = ConfigParser::parseMaxBodySize(strLocationBlock);
	ConfigParser::parseErrorPages(strLocationBlock, this->_errorPages);
	ConfigParser::parseRedirections(strLocationBlock, this->_redirections);
	ConfigParser::parseAllowedMethods(strLocationBlock, this->_methodsAllowed);
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

std::string	ServerLocation::getRedirection(std::string url) const
{
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

size_t	ServerLocation::getIndexVectorSize(void) const
{
	return (this->_indexFiles.size() );
}


LocationStatic::LocationStatic(void)
{
	this->_autoIndex = false;
}


LocationStatic::LocationStatic(std::vector<std::string> strLocationBlock): ServerLocation(strLocationBlock)
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


std::ostream 	&operator<<(std::ostream &out, const LocationStatic &locationStatic)
{
	size_t	indexVectorSize;

	out << "\tLocation: " << locationStatic.getLocation() << std::endl;
	out << "\tRoot: " << locationStatic.getRootDir() << std::endl;

	out << "\tIndex files: " << std::endl;
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

	out << "\tAutoindex: " << locationStatic.getAutoIndex() << std::endl;

	return (out);
}
