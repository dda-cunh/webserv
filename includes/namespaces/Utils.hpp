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

	std::string	lowerStr(std::string const&);
	void		log(std::string const&, LogLevel const&);
}
