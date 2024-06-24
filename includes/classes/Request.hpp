#pragma once

#include "../webserv.hpp"

#define NO_SUCH_HEADER	""

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
		std::string const&	getUri()			const;
		HTTP_METHOD const&	getMethod()			const;
		Socket const&		getClientFD()		const;

		std::string const	getHeader(std::string);
		void				read();
		void				parseBody(std::ustring const&);

		std::string			gNLClient();

	private:
		typedef std::map<long, std::string>	LongStrMap;

		ServerConfig const	_server_config;
		Socket const		_client_fd;
		long const			_unique_id;

		HTTP_VERSION		_version;
		std::ustring		_body;
		std::string			_uri;
		HTTP_METHOD			_method;
		StrStrMap			_headers;

		Request();
		Request(Request const & src);
		Request & operator=(Request const & rhs);

		static char* const	_expected_version;
		static LongStrMap	_client_buffers;

		static long	genUniqueID(Socket const&);
		static void	lowerStr(std::string &);
};
