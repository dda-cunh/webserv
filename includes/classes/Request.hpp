#pragma once

#include "../webserv.hpp"

#define NO_SUCH_HEADER	""

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

		ServerConfig const&			getServerConfig()	const;
		HTTP_VERSION const&			getVersion()		const;
		std::ustring const&			getBody()			const;
		ResponseFlag const&			getFlag()			const;
		std::string const&			getUri()			const;
		HTTP_METHOD const&			getMethod()			const;
		Socket const&				getClientFD()		const;

		std::string const			getHeader(std::string const&);

	private:
		ServerConfig const			_server_config;
		Socket const				_client_fd;

		HTTP_VERSION				_version;
		std::ustring				_body;
		ResponseFlag				_flag;
		std::string					_uri;
		HTTP_METHOD					_method;
		StrStrMap					_headers;

		Request();
		Request(Request const & src);
		Request & operator=(Request const & rhs);

		std::ustring				getNextChunkClient();
		void						putHeader(std::string const&,
										std::string const&);
		void						parseHeaderLine(std::stringstream &);
		void						parseBody(std::ustring const&);
		void						readClient();

		static unsigned int const	_max_request_size;
		static std::string const	_expected_version;

		static HTTP_VERSION			identifyHTTPVersion(std::string const&);
		static std::string const	seekCRLF(std::ustring const&,
										std::ustring::size_type &);
		static std::string			lowerStr(std::string const&);
		static HTTP_METHOD			identifyMethod(std::string const&);
};
