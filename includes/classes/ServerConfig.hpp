#pragma once

#include "../webserv.hpp"

class	ServerConfig
{
	public:
		ServerConfig();
		~ServerConfig(void);

	private:
		uint32_t		ipv4;
		uint16_t		port;

		std::string 	rootDir;
		ServerLocation	*location; // maybe make this a container...?
};

class	ServerLocation
{
	public:
		ServerLocation();
		~ServerLocation(void);

		//	WILL NEED SOME KIND OF FLAGS TO DISTINGUISH BETWEEN STATIC SITE, CGI OR REVERSE PROXY

	private:
		std::string	siteLocation;
		std::string	rootDir;
};