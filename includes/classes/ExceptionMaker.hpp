#pragma once

# include <string>

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
