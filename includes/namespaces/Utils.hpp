#pragma once

#include <cstring>
#include <sys/stat.h>
#include <cstdarg>
#include <string>
#include <fstream>

#ifndef PRODUCTION
# define PRODUCTION 0
#endif

#define LOG(message, level) logImpl(message, level, __FILE__, __LINE__)

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
		L_CGI
	}	LOCATION_BLOCK_TYPE;

	const char PATH_SEPARATOR = '/';

	std::string::size_type	sCountChar(std::string const&, char const&);
	std::string 			concatenatePaths(const std::string basePath, ...);
	std::string				intToString(int const& value);
	std::string				lowerStr(std::string const&);
	std::string				sTrim(std::string const&);
	std::string				pathBaseName(char const*);
	std::string				getCurrentDate();
	size_t					sWordCount(std::string line);
	void					logImpl(std::string const&, LogLevel const&,
									std::string const&, int const&);
	bool 					resourceExists(std::string const &uri);
	bool					isDirectory(std::string const& uri);
	int						stringToInt(const std::string& str);
}
