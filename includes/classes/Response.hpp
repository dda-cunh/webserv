#pragma once

#include "../webserv.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "ServerLocation.hpp"
#include "CGIHandler.hpp"

#include <vector>
#include <algorithm>

class Response {
	public:
		Response(Request const &, ServerConfig const &);
		~Response();
		std::string				const &getResponse() const;
		
		void					setBody(const std::string &body);
		void					setHeader(const std::string &key, const std::string &value);

		ServerLocation *	    getMatchedLocation();
	private:
		Http::STATUS_CODE		_statusCode;
		StrStrMap				_headers;
		std::string				_body;
		std::string				_response;
		Request					_request;
		ServerConfig const &	_serverConfig;
		ServerLocation *		_matchedLocation;
		std::string				_redirectionPath;

		void					dispatchMethod();
		void					parseUri();
		std::string				parseIndex(const std::string &uri);
		void 					handleGETMethod();
		void					handlePOSTMethod();
		void					handleDELETEMethod();
		void					handleMethodNotAllowed();
		void					handleStatic();
		void					handleCGI();

		void					setMatchedLocation();
		void					setCommonHeaders();
		void					setResponse();
		void					setStatusAndReadResource(Http::STATUS_CODE statusCode, std::string uri = "");

		void					readResource(const std::string &uri, bool isErrorResponse = false);
		std::string				getHeadersStr();
		bool					isRedirection();
		void					handleRedirection();
	};
