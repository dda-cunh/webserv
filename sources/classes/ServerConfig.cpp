#include "../../includes/classes/ServerConfig.hpp"

	/*	CONSTRUCTORS	*/
//	MAYBE DEFAULT VALUES COULD BE DEFINED AS MACROS?
ServerConfig::ServerConfig(void)
{
	this->_host = Network::sToIPV4Packed("127.0.0.1");
	this->_port = 80;
	this->_serverName = "example.com";

	this->_rootDir = "/";

	this->_locationBlocks.push_back(new LocationStatic);
}

ServerConfig::ServerConfig(std::vector<std::string> strServerBlock)
{
	//	PARSE STRINGS TO SET MEMBERS (MAYBE USE CONFIGPARSER FOR THIS...)
}

ServerConfig::ServerConfig(const ServerConfig &serverConfig)
{
	if (this != &serverConfig)
		this = serverConfig;
}

ServerConfig::~ServerConfig(void)
{
	size_t	nLocations;

	nLocations = this->_locationBlocks.size();
	for (size_t i = 0; i < nLocations; i++)
		delete this->_locationBlocks.at(i);
}

ServerConfig ServerConfig::&operator=(const ServerConfig &serverConfig)
{
	
}

	/*	GETTERS	*/
std::string	ServerConfig::getServerName(void)
{
	return (this->_serverName);
}

std::string	ServerConfig::getRootDir(void)
{
	return (this->_rootDir);
}

unsigned int	ServerConfig::getMaxBodySize(void)
{
	return (this->_maxBodySize);
}

ServerLocation	ServerConfig::*getLocationFromPath(std::string path)
{
	size_t	nLocations;

	nLocations = this->_locationBlocks.size();
	for (size_t i = 0; i < nLocations; i++)
	{
		if (this->_locationBlocks.at(i)->location == path)
			return (this->_locationBlocks.at(i));
	}
	return (NULL);
}

int	ServerConfig::getLocationType(ServerLocation *location)
{
	if (dynamic_cast<LocationStatic *>(location) != NULL)
		//	return enum for static
	else if (dynamic_cast<LocationCGI *>(location) != NULL)
		//	return enum for CGI
	else
		//	return error
}
