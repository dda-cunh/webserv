#include "ServerConfig.hpp"

	/*	CONSTRUCTORS	*/

ServerConfig::ServerConfig(void)
{
	this->_host = Network::sToIPV4Packed("127.0.0.1");
	this->_port = 80;
	this->_serverName = "";

	this->_locationBlocks.push_back(new LocationStatic);
}

/*
ServerConfig::ServerConfig(std::vector<std::string> strServerBlock)
{
	//	PARSE STRINGS TO SET MEMBERS (MAYBE USE CONFIGPARSER FOR THIS...)
}
*/

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
	size_t	lbSize;

	this->_host = serverConfig.getHost();
	this->_port = serverConfig.getPort();
	this->_serverName = serverConfig.getServerName();
	this->_maxBodySize = serverConfig.getMaxBodySize();

	lbSize = serverConfig.getLocationBlocksSize();
	for (size_t i = 0; i < lbSize; i++)
		this->_locationBlocks.push_back(new ServerLocation(serverConfig.getLocationFromIndex(i) ) );	//	DOES INHERITANCE WORK WITH THIS?

	return (*this);
}

	/*	GETTERS	*/

uint32_t	ServerConfig::getHost(void)
{
	return (this->_host);
}

uint16_t	ServerConfig::getPort(void)
{
	return (this->_port);
}

std::string	ServerConfig::getServerName(void)
{
	return (this->_serverName);
}

unsigned int	ServerConfig::getMaxBodySize(void)
{
	return (this->_maxBodySize);
}

size_t	ServerConfig::getLocationBlocksSize(void)
{
	return (this->_locationBlocks.size());
}

ServerLocation	ServerConfig::*getLocationFromIndex(size_t i)
{
	return (this->_locationBlocks.at(i));
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

LOCATION_BLOCK_TYPE	ServerConfig::getLocationType(ServerLocation *location)
{
	if (dynamic_cast<LocationStatic *>(location) != NULL)
		return (L_STATIC);
//	else if (dynamic_cast<LocationRevProxy *>(location) != NULL)
//		return (L_REV_PROXY);
//	else if (dynamic_cast<LocationCGI *>(location) != NULL)
//		return (L_CGI);
	else
		return (L_UNHANDLED);
}

std::ostream	&operator<<(std::ostream &out, const ServerConfig &serverConfig)
{
	size_t	lbSize;

	out << "Host: " << serverConfig.getHost() << std::endl;
	out << "Port: " << serverConfig.getPort() << std::endl;
	out << "Server name: " << serverConfig.getserverName() << std::endl;

	out << "Max body size: " serverConfig.getmaxBodySize();	//	MOVE THIS TO LOCATIONBLOCK?
	
	lbSize = serverConfig.getLocationBlocksSize();
	for (size_t i = 0; i < lbSize; i++)
	{
		out << "LocationBlock nr. " << i << ":" << std::endl;
		out << *(serverConfig.getLocationFromIndex(i)) << std::endl;
		out << std::endl;
	}
	return (out);
}