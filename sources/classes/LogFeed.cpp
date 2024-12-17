#include "../../includes/classes/LogFeed.hpp"
#include <unistd.h>

LogFeed::LogFeed(void)	throw()
{}

LogFeed::~LogFeed(void)	throw()
{}

LogFeed &	LogFeed::getInstance(void)	throw()
{
	static LogFeed	instance;

	return (instance);
}

void	LogFeed::buff(std::string const& msg, Level l)	throw()
{
	this->_feed.push(Utils::logify(msg, l));
}

void	LogFeed::buff(char const* msg, Level l)	throw()
{
	this->_feed.push(Utils::logify(msg, l));
}

void	LogFeed::log(void)	throw()
{
	if (!this->_feed.empty())
	{
		write(STDOUT_FILENO, this->_feed.front().c_str(), this->_feed.front().length());
		this->_feed.pop();
	}
}
