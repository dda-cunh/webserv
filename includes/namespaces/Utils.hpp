#pragma once

#include <stdint.h>
#include <string>

#ifndef PRODUCTION
# define PRODUCTION 0
#endif

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
	std::string				logify(std::string const&, LogLevel const&);
	std::string				intToString(int const& value);
	std::string				lowerStr(std::string const&);
	std::string				sTrim(std::string const&);
	std::string				pathBaseName(char const*);
	std::string				getCurrentDate();
	uint64_t				pack32sTo64(uint32_t const& msb,
										uint32_t const& lsb);
	uint32_t				get32From64(uint64_t const& l,
										bool const& msb);
	size_t					sWordCount(std::string line);
	bool 					resourceExists(std::string const &uri);
	bool					isDirectory(std::string const& uri);
	int						stringToInt(const std::string& str);
}
