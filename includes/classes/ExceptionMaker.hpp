#ifndef EXCEPTIONMAKER_HPP
# define EXCEPTIONMAKER_HPP

# include <exception>
# include <string>

#include "../webserv.hpp"

class ExceptionMaker : public std::exception
{
	public:
		~ExceptionMaker()			throw();

		ExceptionMaker(const std::string message);

		const char* what()	const	throw();

		void	log();

	protected:
		std::string	_message;
};

#endif
