#include <algorithm>
#include <sstream>

#include "../../includes/namespaces/Network.hpp"
#include "../../includes/namespaces/Utils.hpp"

#include "../../includes/classes/ServerLocation.hpp"
#include "../../includes/classes/ExceptionMaker.hpp"
#include "../../includes/classes/SyntaxChecker.hpp"
#include "../../includes/classes/ServerConfig.hpp"

/*
ServerConfig::ServerConfig(void)
{
	this->_host = Network::sToIPV4Packed("127.0.0.1");
	this->_port = 80;
	this->_serverNames.push_back("webserv.ft");

	this->_locationBlocks.push_back(new ServerLocation);
}
*/
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

			this->_locationBlocks.push_back(new ServerLocation(strLocationBlock) );

			strLocationBlock.clear();
		}
	}
	if (this->_locationBlocks.empty() )
		this->_locationBlocks.push_back(new ServerLocation);
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
	size_t			vectorSize;

	this->_host = serverConfig.getHost();
	this->_port = serverConfig.getPort();

	vectorSize = serverConfig.getServerNamesSize();
	for (size_t i = 0; i < vectorSize; i++)
		this->_serverNames.push_back(serverConfig.getServerName(i) );

	vectorSize = serverConfig.getLocationBlocksSize();
	for (size_t i = 0; i < vectorSize; i++)
		this->_locationBlocks.push_back(new ServerLocation(*serverConfig.getLocationFromIndex(i) ) );

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
		out << *location << std::endl;
		out << std::endl;
	}
	return (out);
}

std::ostream 	&operator<<(std::ostream &out, const ServerLocation &location)
{
	size_t	indexVectorSize;

	out << "\tLocation: " << location.getLocation() << std::endl;
	out << "\tRoot: " << location.getRootDir() << std::endl;
  
	out << "\tIndex files:" << std::endl;
	indexVectorSize = location.getIndexVectorSize();
	for (size_t i = 0; i < indexVectorSize; i++)
		out << "\t\t" << location.getIndexFileName(i) << std::endl;

	out << "\tMax body size: " << location.getMaxBodySize() << std::endl;

	out << "\tError pages:" << std::endl;
	for (IntStrMap::const_iterator itt = location.getErrPageIttBegin(); itt != location.getErrPageIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tRedirections:" << std::endl;
	for (StrStrMap::const_iterator itt = location.getRedirectionIttBegin(); itt != location.getRedirectionIttEnd(); itt++)
		out << "\t\t" << itt->first << " " << itt->second << std::endl;

	out << "\tAllowed methods:" << std::endl;
	for (size_t i = 0; i < location.getMethodsAllowedSize(); i++)
		out << "\t\t" << location.getMethodByIndex(i) << std::endl;

	out << "\tUpload path: " << location.getUploadPath() << std::endl;

	out << "\tAutoindex: " << location.getAutoIndex() << std::endl;

	out << "\tCGI extensions:" << std::endl;
	for (StrArr::const_iterator itt = location.getCgiExtensionsBegin(); itt != location.getCgiExtensionsEnd(); itt++)
		out << "\t\t" << *itt << std::endl;

	return (out);
}
