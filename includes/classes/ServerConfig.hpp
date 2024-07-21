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
					IMPLEMENT ENUM FOR DIFFERENT TYPES OF SERVER LOCATION
		*/

		//			GETTERS
		std::string		getServerName();
		std::string		getRootDir();
		unsigned int	getMaxBodySize();
		//	=============================

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

	protected:											//	KEYWORDS
		std::string					location;			//		location [...] {
		std::string					rootDir;			//		root
		std::string					indexDir;			//		index
		bool						autoIndex;			//		autoindex
		std::vector<Http::METHOD>	methodsAllowed;		//		allowed_methods
		StrStrMap					redirections;		//	
		IntStrMap 					errorPages;			//	
		std::string					uploadDirectory;	//	
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