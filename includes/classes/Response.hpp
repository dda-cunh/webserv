#pragma once

#include "../webserv.hpp"
# include "Request.hpp"

#include <vector>
#include <algorithm> // std::find

#define NO_SUCH_HEADER ""

/**
 *	@brief Represents an HTTP response given by the server.
 */
class Response
{
	public:
		~Response();

		std::string 			const &response() const;

		Response(Request const &);

	private:
		Http::STATUS_CODE		_statusCode;
		StrStrMap 				_headers;
		std::string 			_body;
		std::string 			_response;
		Request					_request;
		IntStrMap				_error_pages;
		StrStrMap				_redirections;

		Response();
		Response(Response const &src);
		Response &operator=(Response const &rhs);

		void 					dispatchMethod();
		void 					handleGETMethod();
		void 					handlePOSTMethod();
		void 					handleDELETEMethod();
		void 					handleMethodNotAllowed();

		void 					setLocation();
		void					setErrorPages();
		void 					setHeader(const std::string &, const std::string &);
		void 					setCommonHeaders();
		void 					setResponse();

		void					readResource(std::string uri);
		void					setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri = "");
		std::string 			getResponseWithoutBody(); // TODO: Debug function, to be removed
		bool 					isRedirection();
		void					handleFileList();
};