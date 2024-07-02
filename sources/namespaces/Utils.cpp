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

		start = 0;
		end = str.size();
		while (std::isspace(str[start]))
			start++;
		while (std::isspace(str[end - 1]))
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
}
