#pragma once

#include <string>
#include <sys/stat.h>
#include <fstream>

namespace	Utils
{
	enum LogLevel
	{
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	typedef enum	e_location_block_type
	{
		L_UNHANDLED,
		L_STATIC,
		L_REV_PROXY,
		L_CGI
	}	LOCATION_BLOCK_TYPE;

	const char PATH_SEPARATOR = '/';

	std::string::size_type	sCountChar(std::string const&, char const&);
	size_t					sWordCount(std::string line);
	std::string				lowerStr(std::string const&);
	std::string				sTrim(std::string const&);
	void					log(std::string const&, LogLevel const&);
	std::string				intToString(int const& value);
	std::string				getCurrentDate();
	bool					isDirectory(std::string const& uri);
	bool 					resourceExists(std::string const &uri);
	std::string 			concatenatePaths(const std::string &basePath, const std::string &appendPath);
}	
