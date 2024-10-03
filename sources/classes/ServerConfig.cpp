#include "../../includes/classes/ServerConfig.hpp"

	/*	CONSTRUCTORS	*/

ServerConfig::ServerConfig(void)
{
	this->_host = Network::sToIPV4Packed("127.0.0.1");
	this->_port = 80;
	this->_serverNames.push_back("webserv.ft");

	this->_locationBlocks.push_back(new LocationStatic);
}

ServerConfig::ServerConfig(std::vector<std::string> strServerBlock)
{
	std::vector<std::string>	strLocationBlock;
	std::string					line;
	size_t						sServBlkSize;

	this->_host = this->_setHost(strServerBlock);
	this->_port = this->_setPort(strServerBlock);
	this->_setServerName(strServerBlock, this->_serverNames);

	sServBlkSize = strServerBlock.size();
	for (size_t i = 0; i < sServBlkSize; i++)
	{
		line = strServerBlock.at(i);
		if (line.find("location") == 0)
		{
			while (line.at(line.size() - 1) != '}')
			{
				line = Utils::sTrim(line);
				strLocationBlock.push_back(line);
				line = strServerBlock.at(++i);
			}
			switch (this->parseLocationType(strLocationBlock) )
			{
				case (Utils::L_STATIC):
					this->_locationBlocks.push_back(new LocationStatic(strLocationBlock) );
					break ;
				case (Utils::L_REV_PROXY):
					throw (ExceptionMaker("This feature has not been implemented yet") );
					break ;
				case (Utils::L_CGI):
					throw (ExceptionMaker("This feature has not been implemented yet") );
					break ;
				case (Utils::L_UNHANDLED):
					throw (ExceptionMaker("Invalid Location type") );
					break ;
			}
			strLocationBlock.clear();
		}
	}
	if (this->_locationBlocks.empty() )
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
	ServerLocation	*location;
	size_t	vectorSize;

	this->_host = serverConfig.getHost();
	this->_port = serverConfig.getPort();

	vectorSize = serverConfig.getServerNamesSize();
	for (size_t i = 0; i < vectorSize; i++)
		this->_serverNames.push_back(serverConfig.getServerName(i) );

	vectorSize = serverConfig.getLocationBlocksSize();
	for (size_t i = 0; i < vectorSize; i++)
	{
		location = serverConfig.getLocationFromIndex(i);

		switch (this->getLocationType(location))
		{
			case (Utils::L_STATIC):
				this->_locationBlocks.push_back(new LocationStatic(*(dynamic_cast<LocationStatic*>(location) ) ) );
				break ;
			case (Utils::L_REV_PROXY):
				break ;
			case (Utils::L_CGI):
			//	this->_locationBlocks.push_back(new LocationCGI(location));
				break ;
			case (Utils::L_UNHANDLED):
				throw (ExceptionMaker("Invalid Location type") );
				break ;
		}
	}

	return (*this);
}

	/*	GETTERS	*/

uint32_t	ServerConfig::getHost(void) const
{
	return (this->_host);
}

uint16_t	ServerConfig::getPort(void) const
{
	return (this->_port);
}

std::string	ServerConfig::getServerName(size_t i) const
{
	if (i >= this->_serverNames.size() )
		throw (ExceptionMaker("Server name index is out of bounds") );
	return (this->_serverNames.at(i) );
}

ServerLocation	*ServerConfig::getLocationFromIndex(size_t i) const
{
	if (i >= this->_locationBlocks.size() )
		throw (ExceptionMaker("Location index is out of bounds") );
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

size_t	ServerConfig::getServerNamesSize(void) const
{
	return (this->_serverNames.size() );
}

size_t	ServerConfig::getLocationBlocksSize(void) const
{
	return (this->_locationBlocks.size() );
}


Utils::LOCATION_BLOCK_TYPE	ServerConfig::getLocationType(ServerLocation *location) const
{
	if (dynamic_cast<LocationStatic *>(location) != NULL)
		return (Utils::L_STATIC);
//	else if (dynamic_cast<LocationRevProxy *>(location) != NULL)
//		return (L_REV_PROXY);
//	else if (dynamic_cast<LocationCGI *>(location) != NULL)
//		return (L_CGI);
	else
		return (Utils::L_UNHANDLED);
}



uint32_t	ServerConfig::_setHost(std::vector<std::string> strServerBlock)
{
	size_t		vectorSize;
	std::string	strHost;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			strHost = SyntaxChecker::strParseLine(strServerBlock.at(i) );

			if (strHost.find(':') != strHost.npos)
				return (Network::sToIPV4Packed(strHost.substr(0, strHost.find(':') ) ) );
			else
			{
				if (strHost.find('.') != strHost.npos)
					return (Network::sToIPV4Packed(strHost) );
				else
					return (Network::sToIPV4Packed(DEFAULT_HOST) );
			}
		}
	}

	return (Network::sToIPV4Packed(DEFAULT_HOST) );
}

uint16_t	ServerConfig::_setPort(std::vector<std::string> strServerBlock)
{
	std::string	strPort;
	size_t		vectorSize;
	int			nPort;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		if (strServerBlock.at(i).find("listen") == 0)
		{
			strPort = SyntaxChecker::strParseLine(strServerBlock.at(i) );

			if (strPort.find(':') != strPort.npos)
				strPort = strPort.substr(strPort.find(':') + 1);
			else if (strPort.find('.') != strPort.npos)
				return (DEFAULT_PORT);

			nPort = std::atoi(strPort.c_str() );
			return (static_cast<uint16_t>(nPort) );
		}
	}

	return (DEFAULT_PORT);
}

void	ServerConfig::_setServerName(std::vector<std::string> strServerBlock, std::vector<std::string> &serverNames)
{
	size_t				vectorSize;
	std::string			line;
	std::stringstream	strStream;
	std::string			strArg;

	vectorSize = strServerBlock.size();
	for (size_t i = 0; i < vectorSize; i++)
	{
		line = strServerBlock.at(i);
		if (line.find("server_name") == 0)
		{
			line = SyntaxChecker::strParseLine(line);
			strStream << line;
			while (strStream >> strArg)
			{
				if (std::find(serverNames.begin(), serverNames.end(), strArg) != serverNames.end() )
					throw (ExceptionMaker("Multiple instances of the same argument found in \"server_name\" directive") );
				serverNames.push_back(strArg);
			}
			strStream.clear();
		}
	}
}



Utils::LOCATION_BLOCK_TYPE	ServerConfig::parseLocationType(std::vector<std::string> strLocationBlock)
{
	//	THIS FUNCTION WILL BE IMPLEMENTED LATER
	(void)strLocationBlock;
	return (Utils::L_STATIC);
}




std::ostream	&operator<<(std::ostream &out, const ServerConfig &serverConfig)
{
	ServerLocation	*location;
	size_t	vectorSize;

	out << "Host: " << Network::iPV4PackedTos(serverConfig.getHost() ) << std::endl;
	out << "Port: " << serverConfig.getPort() << std::endl;
	
	vectorSize = serverConfig.getServerNamesSize();
	out << "Server names: " << std::endl;
	for (size_t i = 0; i < vectorSize; i++)
		out << "\t" << serverConfig.getServerName(i) << std::endl;
	
	vectorSize = serverConfig.getLocationBlocksSize();
	for (size_t i = 0; i < vectorSize; i++)
	{
		location = serverConfig.getLocationFromIndex(i);
		out << "LocationBlock nr. " << i << ":" << std::endl;

		switch (serverConfig.getLocationType(location))
		{
			case (Utils::L_STATIC):
				out << *(dynamic_cast<LocationStatic *>(location) ) << std::endl;
				break ;
			case (Utils::L_REV_PROXY):
				break ;
			case (Utils::L_CGI):
				// out << *(dynamic_cast<LocationCGI *>(location) ) << std::endl;
				break ;
			case (Utils::L_UNHANDLED):
				//	THROW EXCEPTION
				break ;
		}		
		out << std::endl;
	}
	return (out);
}
