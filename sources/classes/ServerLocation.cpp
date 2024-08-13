#include "../../includes/classes/ServerLocation.hpp"

//	BASE CLASS

ServerLocation::ServerLocation(void)
{
	this->_location = "/";
	this->_rootDir = "./test_files/error_pages/"
	this->_indexFile = "index.html";	//	THIS FILE NEEDS TO BE CREATED!!!
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

std::string	ServerLocation::getLocation(void)
{
	return (this->_location);
}

std::string	ServerLocation::getRootDir(void)
{
	return (this->_rootDir);
}

std::string	ServerLocation::getIndexDir(void)
{
	return (this->_indexDir);
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

LocationStatic::~LocationStatic(void)
{
	return ;
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

/*
LocationRevProxy::LocationRevProxy(std::vector<std::string> strLocationBlock)
{

}
*/

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

/*
LocationCGI::LocationCGI(std::vector<std::string> strLocationBlock)
{

}
*/

LocationCGI::~LocationCGI(void)
{
	return ;
}

