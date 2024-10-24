#pragma once

#include "../webserv.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "ServerLocation.hpp"
#include "CGIHandler.hpp"
#include "CGIMatch.hpp"

#include <vector>
#include <algorithm>

class Response {
	public:
		Response(Request const &, ServerBlocks const &);
		~Response();
		std::string				const &getResponse() const;
		
		void					setBody(const std::string &body);
		void					setHeader(const std::string &key, const std::string &value);
		void					setStatusCode(Http::STATUS_CODE statusCode);

		ServerLocation *	    getLocationMatch();
		Request const &			getRequest() const;
		CGIMatch const &		getCGIMatch() const;

	private:
		Http::STATUS_CODE		_statusCode;
		StrStrMap				_headers;
		std::string				_body;
		std::string				_response;
		Request					_request;
		ServerBlocks const &	_serverBlocks;
		ServerLocation *		_locationMatch;
		std::string				_redirectionPath;
		CGIMatch 				_cgiMatch;
		
		void					dispatchMethod();
		void 					handleGETMethod();
		void					handlePOSTMethod();
		void					handleDELETEMethod();
		void					handleMethodNotAllowed();
		void					handleCGI();

		void					setMatchedLocation();
		void					setCommonHeaders();
		void					setResponse();
		void					setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri = "");

		void					readResource(const std::string &uri, bool isErrorResponse = false);
		std::string				getHeadersStr();
		bool					isRedirection();
		void					handleRedirection();

		void					setCGIMatch();
	};
