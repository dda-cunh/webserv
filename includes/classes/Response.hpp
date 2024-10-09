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
		~Response();
		std::string const &getResponse() const;
		Response(Request const &, ServerBlocks const &);

	private:
		Http::STATUS_CODE _statusCode;
		StrStrMap _headers;
		std::string _body;
		std::string _response;
		Request _request;
		ServerBlocks const &_serverBlocks;
		ServerLocation *_matchedLocation;
		std::string _redirectionPath;

		void dispatchMethod();
		void handleGETMethod();
		void handlePOSTMethod();
		void handleDELETEMethod();
		void handleMethodNotAllowed();
		void handleCGI();

		void setMatchedLocation();
		void setHeader(const std::string &, const std::string &);
		void setCommonHeaders();
		void setResponse();
		void setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri = "");

		void readResource(const std::string &uri, bool isErrorResponse = false);
		std::string getHeadersStr();
		bool isRedirection();
		void handleRedirection();
		void handleFileList();
};
