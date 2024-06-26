#include "../../includes/webserv.hpp"

namespace Utils
{
	std::string	lowerStr(std::string const& str)
	{
		std::string	lower_str(str);

		for (std::string::iterator it = lower_str.begin(); it != lower_str.end(); it++)
			*it = std::tolower(*it);
		return (lower_str);
	}

	void	log(std::string const& message, LogLevel const& level)
	{
		std::string	level_str;

		switch (level)
		{
			case LOG_INFO:
				level_str = CLI_COLORS_GREEN "INFO" CLI_COLORS_RESET;
				break ;
			case LOG_WARNING:
				level_str = CLI_COLORS_YELLOW "WARNING" CLI_COLORS_RESET;
				break ;
			case LOG_ERROR:
				level_str = CLI_COLORS_RED "ERROR" CLI_COLORS_RESET;
				break ;
		}
		std::cerr << level_str << ": " << message << std::endl;
	}
}
