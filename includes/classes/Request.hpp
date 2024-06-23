#pragma once

#include "../webserv.hpp"

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

		ServerConfig const&	getServerConfig()	const;
		HTTP_VERSION const&	getVersion()		const;
		std::ustring const&	getBody()			const;
		std::ustring const&	getUri()			const;
		HTTP_METHOD const&	getMethod()			const;
		Socket const&		getClientFD()		const;

		std::string const	getHeader(std::string);
		void				read();
		void				parseBody(std::ustring const&);

	private:
		ServerConfig const	_server_config;
		Socket const		_client_fd;

		HTTP_VERSION		_version;
		std::ustring		_body;
		std::ustring		_uri;
		HTTP_METHOD			_method;
		StrStrMap			_headers;

		Request();
		Request(Request const & src);
		Request & operator=(Request const & rhs);

		static void	lowerStr(std::string &);
};
