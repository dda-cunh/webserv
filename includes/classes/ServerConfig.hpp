#pragma once

#include <fstream>
#include <vector>
#include <map>
#include "../webserv.hpp"

//	USE SOMETHING LIFO
typedef std::vector<ServerLocation *>	LocationBlocks;

class	ServerConfig
{
	public:
		ServerConfig();			//	DONT FORGET TO SET DEFAULTS
		~ServerConfig(void);	//	DESTRUCTOR MUST DELETE POINTERS IN locationBlocks
		
		std::string		getServerName();
		std::string		getRootDir();
		unsigned int	getMaxBodySize();

		LocationBlocks	locationBlocks;

	private:
		uint32_t		host;
		uint16_t		port;

		std::string		_serverName;
		std::string 	_rootDir;	//	TEST ON NGINX TO SEE HOW TO HANDLE THIS ONE
		unsigned int	_maxBodySize;
};

class	ServerLocation
{
	public:
				ServerLocation(void);
				ServerLocation(/* pass configs as struct */);
		virtual ~ServerLocation(void);

		std::string	getLocation();
		std::string	getRootDir();
		std::string	getindexDir();
		bool		getAutoIndex();
		
		bool		methodIsAllowed(std::string method);

	protected:
		std::string	location;
		std::string	rootDir;
		std::string	indexDir;
		bool		autoIndex;
		std::map<std::string, bool>	methodsAllowed;
};

//	OBJECTS OF THESE CLASSES CAN BE IDENTIFIED WITH DYNAMIC_CAST
class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic();
		~LocationStatic();
	
};

class	LocationRevProxy: public ServerLocation
{
	public:
		LocationRevProxy();
		~LocationRevProxy();

	private:
		//	proxy_pass
	
};

class LocationCGI: public ServerLocation
{
	public:
		LocationCGI();
		~LocationCGI();
}