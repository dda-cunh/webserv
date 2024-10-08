#pragma once

#include "../webserv.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "ServerLocation.hpp"

#include <vector>
#include <algorithm> // std::find

/**
 *	@brief Represents an HTTP response given by the server.
 */
class Response
{
	public:
		typedef std::vector<ServerConfig> ServerBlocks;

		~Response();

		std::string const &response() const;

		Response(Request const &, ServerBlocks const &);

	private:
		Http::STATUS_CODE _statusCode;
		StrStrMap _headers;
		std::string _body;
		std::string _response;
		Request _request;
		IntStrMap _error_pages;
		StrStrMap _redirections;
		ServerBlocks const &_serverBlocks;

		ServerLocation *_matchedLocation;

		Response();
		Response(Response const &src);
		Response &operator=(Response const &rhs);

		void dispatchMethod();
		void handleGETMethod();
		void handlePOSTMethod();
		void handleDELETEMethod();
		void handleMethodNotAllowed();

		void setLocation();
		void setErrorPages();
		void setHeader(const std::string &, const std::string &);
		void setCommonHeaders();
		void setResponse();

		void readResource(const std::string &uri, bool isErrorResponse = false);
		void setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri = "");
		std::string getResponseWithoutBody(); // TODO: Debug function, to be removed
		bool isRedirection();
		void handleFileList();
};
