#pragma once

#include "../webserv.hpp"

typedef enum	e_response_flag
{
	NO_FLAG,
	_400
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

		Request(ServerConfig const&, Socket const&);

		Http::VERSION const&		getVersion()		const;
		ServerConfig const&			getServerConfig()	const;
		Http::METHOD const&			getMethod()			const;
		std::ustring const&			getBody()			const;
		ResponseFlag const&			getFlag()			const;
		std::string const&			getUri()			const;
		Socket const&				getClientFD()		const;

		std::string const			getHeader(std::string const&);

		static std::string const	_no_such_header;

	private:
		ServerConfig const			_server_config;
		Socket const				_client_fd;

		Http::VERSION				_version;
		Http::METHOD				_method;
		std::ustring				_body;
		ResponseFlag				_flag;
		std::string					_uri;
		StrStrMap					_headers;

		Request & operator=(Request const & rhs);
		Request(Request const & src);
		Request();

		std::ustring				getNextChunkClient();
		void						putHeader(std::string const&,
										std::string const&);
		void						parseHeaderLine(std::stringstream &);
		void						parseBody(std::ustring const&);
		void						readClient();

		static unsigned int const	_max_request_size;

		static std::string			seekCRLF(std::ustring const&,
										std::ustring::size_type &);
};
