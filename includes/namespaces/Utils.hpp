#pragma once

#include <string>

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
	void					log(std::string const&, LogLevel const&);
}
