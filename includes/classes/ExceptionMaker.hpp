#ifndef EXCEPTIONMAKER_HPP
# define EXCEPTIONMAKER_HPP

# include <exception>
# include <string>

class ExceptionMaker : public std::exception
{
	public:
		~ExceptionMaker()			throw();

		ExceptionMaker(const std::string message);

		const char* what()	const	throw();

	protected:
		std::string	_message;
};

#endif
