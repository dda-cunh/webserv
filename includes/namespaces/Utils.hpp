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

	std::string::size_type	sCountChar(std::string const&, char const&);
	std::string				lowerStr(std::string const&);
	std::string				sTrim(std::string const&);
	void					log(std::string const&, LogLevel const&);
	std::string				intToString(int const& value);
	std::string				getCurrentDate();
	bool					isDirectory(std::string const& uri);
	bool 					resourceExists(std::string const &uri);
}	
