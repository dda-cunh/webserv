#pragma once

#include <string>
#include <queue>

#include "../../includes/namespaces/Utils.hpp"

#define LOGFEED								LogFeed::getInstance()

/**
 * @class LogFeed
 * @brief Singleton class for logging messages.
 * 
 * The LogFeed class provides a mechanism to buffer and log messages. It follows the singleton design pattern to ensure
 * that only one instance of the class exists throughout the application.
 */
class LogFeed
{
	private:
		std::queue<std::string>	_feed;
		LogFeed()													throw();
	
		typedef Utils::LogLevel Level;

	public:
		~LogFeed()													throw();

		static LogFeed &	getInstance()							throw();
		void				buff(std::string const & msg, Level l)	throw();
		void				buff(char const * msg, Level l)			throw();
		void				log()									throw();
		void				dump()									throw();
};
