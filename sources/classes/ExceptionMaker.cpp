#include "../../includes/classes/ExceptionMaker.hpp"
#include "../../includes/classes/LogFeed.hpp"

ExceptionMaker::~ExceptionMaker()	throw()
{

}

ExceptionMaker::ExceptionMaker(const std::string message)
	: _message(message)
{

}

const char *ExceptionMaker::what()	const	throw()
{
	return (this->_message.c_str());
}

void ExceptionMaker::log()
{
	LogFeed::getInstance().buff(_message, Utils::LOG_ERROR);
}
