#pragma once

#include "../webserv.hpp"

typedef enum	e_response_flag
{
	NO_FLAG,
	_400,
	_EMPTY
}	ResponseFlag;

/**
 *	@brief Represents an HTTP request received by the server.
 *	Depends on a ServerConfig instance and client_fd.
 *	Should only be instanciated if client is ready to be read.
 */
class Request
{
	public:
		Request & operator=(Request const & rhs);
		Request(Request const & src);
		Request(ByteArr const&);
		~Request();
		Request();

		Http::VERSION const&		version()					const;
		Http::METHOD const&			method()					const;
		ResponseFlag const&			flag()						const;
		std::string const&			uri()						const;
		ByteArr const&				body()						const;
		int const&					clientFD()					const;
		std::string 				header(std::string const&)	const;
		std::string					str()						const;

		std::string					seekCRLF(ByteArr const&,
										ByteArr::size_type &);

		static std::string const	_no_such_header;

	private:
		Http::VERSION				_version;
		Http::METHOD				_method;
		ResponseFlag				_flag;
		std::string					_uri;
		StrStrMap					_headers;
		ByteArr						_body;

		void						putHeader(std::string const&,
										std::string const&);
		void						parseHeaderLine(std::string const&);
		void						parseBody(ByteArr const&);

		static unsigned int const	_max_request_size;
};
