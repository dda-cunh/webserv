#pragma once

#include "../webserv.hpp"

class Request
{
	public:
		~Request(void);

		Request(ServerConfig const&, Socket const&);

		ServerConfig const&	getServerConfig()	const;
		HTTP_VERSION const&	getVersion()		const;
		std::ustring const&	getBody()			const;
		std::ustring const&	getUri()			const;
		HTTP_METHOD const&	getMethod()			const;
		Socket const&		getClientFD()		const;
		std::ustring		getHeader()			const;

		std::ustring		parseBody(std::ustring const&);

	private:
		ServerConfig const	_server_config;
		HTTP_VERSION const	_version;
		std::ustring const	_body;
		std::ustring const	_uri;
		UStrUStrMap const	_headers;
		HTTP_METHOD const	_method;
		Socket const		_client_fd;

		Request(void);
		Request(Request const & src);
		Request & operator=(Request const & rhs);
};
