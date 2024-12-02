#pragma once

#include "../webserv.hpp"

#define TRANSFER_ENCODING_CHUNKED -1

typedef enum	e_response_flag
{
	NO_FLAG,
	_400,
	_EMPTY
}	ResponseFlag;

typedef enum	e_req_parsed_stage
{
	REQ_PARSED_NONE,
	REQ_PARSED_FIRST_LINE,
	REQ_PARSED_HEADERS,
	REQ_PARSED_BODY
}	ReqParsedStage;

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
		~Request();
		Request();

		ReqParsedStage const&		parsingStage()				const;
		Http::VERSION const&		version()					const;
		Http::METHOD const&			method()					const;
		ResponseFlag const&			flag()						const;
		std::string const&			uri()						const;
		std::string const&			body()						const;
		int const&					clientFD()					const;
		std::string 				header(std::string const&)	const;
		std::string					str()						const;

		std::istream &				getCRLF(std::istream & input,
											std::string & output);
		void						doParse(std::string requestBytes);

		static std::string const	_no_such_header;

	private:
		ReqParsedStage				_parsing_stage;
		Http::VERSION				_version;
		Http::METHOD				_method;
		ResponseFlag				_flag;
		std::string					_parse_feed;
		std::string					_body;
		std::string					_uri;
		StrStrMap					_headers;
		long						_content_len;

		void						putHeader(std::string const&,
										std::string const&);
		void						parseHeaderLine(std::string const&);
		bool						parseBody(std::string const&);

};
