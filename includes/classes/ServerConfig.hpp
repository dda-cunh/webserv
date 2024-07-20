#pragma once

#include <fstream>
#include <vector>
#include <map>
#include "../webserv.hpp"

//	USE SOMETHING LIFO
typedef std::vector<ServerLocation *>	LocationBlocks;

// ADD ENUM FOT LOCATION TYPES

class	ServerConfig
{
	public:
		ServerConfig(void);			//	DONT FORGET TO SET DEFAULTS
		ServerConfig(std::vector<std::string> strLocationBlock);
		~ServerConfig(void);	//	DESTRUCTOR MUST DELETE POINTERS IN locationBlocks

		/*
			ADD THESE METHODS:
				getLocationFromPath(std::string path) - returns const ref to locationBlock corresponting to path; if not found: return NULL or exception?
					CHECK WITH NGINX HOW PATH TREES ARE HANDLED & HOW TO LIMIT ACCESS TO PATHS
				getLocationType(ServerLocation *location) - return corresponding type from enum
		*/

		/*
			ADD DEFAULT/CUSTOM ERROR PAGES - BOTH ON SERER & LOCATION CONTEXT
				> 404
				> 502
		*/

		std::string		getServerName();
		std::string		getRootDir();
		unsigned int	getMaxBodySize();

		LocationBlocks	locationBlocks;

	private:
		uint32_t		host;
		uint16_t		port;	//	CHECK FOR OVERFLOWS

		std::string		_serverName;
		std::string 	_rootDir;	//	TEST ON NGINX TO SEE HOW TO HANDLE THIS ONE
		unsigned int	_maxBodySize;
};

class	ServerLocation
{
	public:
				ServerLocation(void);
				ServerLocation(/* pass configs as struct */);
		virtual ~ServerLocation(void) = 0;

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

/*
			std::vector<Http::METHOD>	_allowedMethods
			StrStrMap					_redirections
			std::string					_root		MANDATORY, else wrong configuration
			bool 						_autoindex
			std::string					_uploadDirectory
			IntStrMap 					_error_pages;
*/

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