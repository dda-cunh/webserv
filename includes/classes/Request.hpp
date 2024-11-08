#pragma once

#include "../webserv.hpp"

typedef enum	e_response_flag
{
	NO_FLAG,
	_400
	//	ADD FLAG FOR EMPTY REQUESTS
}	ResponseFlag;

/**
 *	@brief Represents an HTTP request received by the server.
 *	Depends on a ServerConfig instance and client_fd.
 *	Should only be instanciated if client is ready to be read.
 */
class Request
{
	public:
		~Request();

		Request(Request const & src);
		Request(int const&);

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
		int const					_client_fd;

		Http::VERSION				_version;
		Http::METHOD				_method;
		ResponseFlag				_flag;
		std::string					_uri;
		StrStrMap					_headers;
		ByteArr						_body;

		Request & operator=(Request const & rhs);
		Request();

		ByteArr						getNextChunkClient();
		void						putHeader(std::string const&,
										std::string const&);
		void						parseHeaderLine(std::string const&);
		void						parseBody(ByteArr const&);
		void						readClient();

		static unsigned int const	_max_request_size;
};
