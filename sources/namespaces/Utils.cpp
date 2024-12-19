#include <sys/stat.h>

#include <cstdarg>
#include <sstream>

#include "../../includes/webserv.hpp"

#include "../../includes/namespaces/Utils.hpp"


namespace Utils
{
	std::string::size_type	sCountChar(std::string const& str, char const& c)
	{
		std::string::size_type count;

		count = 0;
		for (std::string::size_type i = 0; i < str.size(); i++)
			if (str[i] == c)
				count++;
		return (count);
	}

	size_t	sWordCount(std::string line)
	{
		std::istringstream	strStream(line);
		std::string			outStr;
		size_t				count;

		count = 0;
		while (strStream >> outStr)
			count++;

		return (count);
	}

	std::string	lowerStr(std::string const& str)
	{
		std::string	lower_str(str);

		for (std::string::iterator it = lower_str.begin(); it != lower_str.end(); it++)
			*it = std::tolower(*it);
		return (lower_str);
	}

	std::string				sTrim(std::string const& str)
	{
		std::string::size_type	start;
		std::string::size_type	end;

		if (str.empty() )
			return (str);

		start = 0;
		end = str.size();
		while (start < end && std::isspace(str[start]))
			start++;
		while (end > start && std::isspace(str[end - 1]))
			end--;
		return (str.substr(start, end - start));
	}

	std::string	logify(std::string const& message, LogLevel const& level)
	{
		std::stringstream	level_ss;

		switch (level)
		{
			case LOG_INFO:
			{
				level_ss << CLI_COLORS_GREEN "INFO\t<-- " CLI_COLORS_RESET;
				level_ss << message << CLI_COLORS_GREEN " -->";
				break ;
			}
			case LOG_WARNING:
			{
				level_ss << CLI_COLORS_YELLOW "WARNING\t<-- " CLI_COLORS_RESET;
				level_ss << message << CLI_COLORS_YELLOW " -->";
				break ;
			}
			case LOG_ERROR:
			{
				level_ss << CLI_COLORS_RED "ERROR\t<-- " CLI_COLORS_RESET;
				level_ss << message << CLI_COLORS_RED " -->";
				break ;
			}
		}
		return (level_ss.str() + CLI_COLORS_RESET + '\n');
	}

	std::string intToString(int const& value)
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
	
	int stringToInt(const std::string& str)
	{
		std::stringstream ss(str);
		int value;
		ss >> value;
		return value;
	}

	std::string getCurrentDate()
	{
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *gmtime(&now);
		strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tstruct);
		return buf;
	}

	bool resourceExists(std::string const &uri)
	{
		struct stat buffer;
		return (stat(uri.c_str(), &buffer) == 0);
	}

	std::string concatenatePaths(const std::string basePath, ...)
	{
		va_list args;
		va_start(args, basePath);
	
		std::string result = basePath;
		if (!result.empty() && result[result.length() - 1] != PATH_SEPARATOR)
			result += PATH_SEPARATOR;
	
		const char* nextPath;
		while ((nextPath = va_arg(args, const char*)) != NULL)
		{
	
			std::string adjustedPath = nextPath;
			if (!adjustedPath.empty() && adjustedPath[0] == PATH_SEPARATOR)
				adjustedPath = adjustedPath.substr(1);
	
			result += adjustedPath;
			if (!result.empty() && result[result.length() - 1] != PATH_SEPARATOR)
				result += PATH_SEPARATOR;
	
		}
	    if (!result.empty() && result[result.length() - 1] == PATH_SEPARATOR)
			result = result.substr(0, result.length() - 1);

		va_end(args);
		return result;
	}

	std::string	pathBaseName(char const* fullPath)
	{
		char const*	start;

		if (!fullPath)
			return (std::string());
		start = fullPath;
		while (*fullPath)
			fullPath++;
		while (fullPath > start)
		{
			if (*fullPath == '/')
			{
				fullPath++;
				break ;
			}
			fullPath--;
		}
		return (std::string(fullPath));
	}

	uint64_t	pack32sTo64(uint32_t const& msb, uint32_t const& lsb)
	{
		return (((uint64_t)msb << 32) | lsb);
	}

	uint32_t	get32From64(uint64_t const& l, bool const& msb)
	{
		if (msb)
			return ((uint32_t)(l >> 32));
		return ((uint32_t)l);
	}
}
