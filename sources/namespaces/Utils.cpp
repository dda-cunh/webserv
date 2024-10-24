#include "../../includes/webserv.hpp"

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

	void	log(std::string const& message, LogLevel const& level)
	{
		std::string	level_str;

		switch (level)
		{
			case LOG_INFO:
			{
				std::cout << CLI_COLORS_GREEN "INFO\t<-- " CLI_COLORS_RESET;
				std::cout << message << CLI_COLORS_GREEN " -->" CLI_COLORS_RESET << '\n';
				return ;
			}
			case LOG_WARNING:
			{
				std::cerr << CLI_COLORS_YELLOW "WARNING\t<-- " CLI_COLORS_RESET;
				std::cerr << message << CLI_COLORS_YELLOW " -->" CLI_COLORS_RESET << '\n';
				break ;
			}
			case LOG_ERROR:
			{
				std::cerr << CLI_COLORS_RED "ERROR\t<-- " CLI_COLORS_RESET;
				std::cerr << message << CLI_COLORS_RED " -->" CLI_COLORS_RESET << '\n';
				break ;
			}
		}
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

	std::string concatenatePaths(const std::string &basePath, ...)
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
}