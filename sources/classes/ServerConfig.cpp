#include "../../includes/classes/ServerConfig.hpp"

ServerConfig::ServerConfig(void)
{
	this->_host = Network::sToIPV4Packed("127.0.0.1");
	this->_port = 80;
	this->_serverName = "webserv.ft";

	this->_locationBlocks.push_back(new LocationStatic);
}

ServerConfig::ServerConfig(const ServerConfig &serverConfig)
{
	if (this != &serverConfig)
		*this = serverConfig;
}

ServerConfig::~ServerConfig(void)
{
	size_t	nLocations;

	nLocations = this->_locationBlocks.size();
	for (size_t i = 0; i < nLocations; i++)
		delete this->_locationBlocks.at(i);
}

ServerConfig &ServerConfig::operator=(const ServerConfig &serverConfig)
{
	size_t	lbSize;

	this->_host = serverConfig.getHost();
	this->_port = serverConfig.getPort();
	this->_serverName = serverConfig.getServerName();

	lbSize = serverConfig.getLocationBlocksSize();

	ServerLocation	*location;
	for (size_t i = 0; i < lbSize; i++)
	{
		location = serverConfig.getLocationFromIndex(i);

		switch (this->getLocationType(location))
		{
			case (L_STATIC):
				this->_locationBlocks.push_back(new LocationStatic(*(dynamic_cast<LocationStatic*>(location) ) ) );
				break ;
			case (L_REV_PROXY):
				break ;
			case (L_CGI):
				break ;
			case (L_UNHANDLED):
				throw (ExceptionMaker("Invalid Location type") );
				break ;
		}
	}

	return (*this);
}


uint32_t	ServerConfig::getHost(void) const
{
	return (this->_host);
}

uint16_t	ServerConfig::getPort(void) const
{
	return (this->_port);
}

std::string	ServerConfig::getServerName(void) const
{
	return (this->_serverName);
}

size_t	ServerConfig::getLocationBlocksSize(void) const
{
	return (this->_locationBlocks.size());
}

ServerLocation	*ServerConfig::getLocationFromIndex(size_t i) const
{
	return (this->_locationBlocks.at(i));
}

ServerLocation	*ServerConfig::getLocationFromPath(std::string path) const
{
	size_t	nLocations;

	nLocations = this->_locationBlocks.size();
	for (size_t i = 0; i < nLocations; i++)
	{
		if (this->_locationBlocks.at(i)->getLocation() == path)
			return (this->_locationBlocks.at(i));
	}
	return (NULL);
}

LOCATION_BLOCK_TYPE	ServerConfig::getLocationType(ServerLocation *location) const
{
	if (dynamic_cast<LocationStatic *>(location) != NULL)
		return (L_STATIC);
	else
		return (L_UNHANDLED);
}

std::ostream	&operator<<(std::ostream &out, const ServerConfig &serverConfig)
{
	size_t	lbSize;

	out << "Host: " << Network::iPV4PackedTos(serverConfig.getHost() ) << std::endl;
	out << "Port: " << serverConfig.getPort() << std::endl;
	out << "Server name: " << serverConfig.getServerName() << std::endl;
	
	lbSize = serverConfig.getLocationBlocksSize();

	ServerLocation	*location;
	for (size_t i = 0; i < lbSize; i++)
	{
		location = serverConfig.getLocationFromIndex(i);
		out << "LocationBlock nr. " << i << ":" << std::endl;
		switch (serverConfig.getLocationType(location))
		{
			case (L_STATIC):
				out << *(dynamic_cast<LocationStatic *>(location) ) << std::endl;
				break ;
			case (L_REV_PROXY):
				break ;
			case (L_CGI):
				break ;
			case (L_UNHANDLED):
				throw (ExceptionMaker("Invalid Location type") );
				break ;
		}		
		out << std::endl;
	}
	return (out);
}
