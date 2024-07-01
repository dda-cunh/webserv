#pragma once

#include "../webserv.hpp"
# include "Request.hpp"
#include <fstream>

#define NO_SUCH_HEADER ""

/**
 *	@brief Represents an HTTP response given by the server.
 */
class Response
{
	public:
		Response();
		~Response();

		std::string 			const &response() const;

		void 					dispatchRequestMethod(Request const &);

	private:
		int 					_statusCode;
		StrStrMap 				_headers;
		std::string 			_body;
		std::string 			_response;

		Response(Response const &src);
		Response &operator=(Response const &rhs);

		void 					handleGETMethod(Request const &);
		void 					handlePOSTMethod(Request const &);
		void 					handleDELETEMethod(Request const &);

		std::					string getHTTPStatus(int statusCode) const;

		void 					setHeader(const std::string &, const std::string &);
		void 					setCommonHeaders();
		void 					setResponse();
};