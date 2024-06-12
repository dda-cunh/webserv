#include "../../includes/classes/ExceptionMaker.hpp"

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
