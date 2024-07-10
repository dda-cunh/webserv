#pragma once

#include "../webserv.hpp"
# include "Request.hpp"
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

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

		Response();
		Response(Response const &src);
		Response &operator=(Response const &rhs);

		void 					dispatchRequestMethod();

		void 					handleGETMethod(Request const &);
		void 					handlePOSTMethod(Request const &);
		void 					handleDELETEMethod(Request const &);

		void					setErrorPages();

		void 					setHeader(const std::string &, const std::string &);
		void 					setCommonHeaders();
		void 					setResponse();

		void					readResource(std::string uri);
		void					listDirectory(std::string &uri);
		std::string 			getResponseWithoutBody(); // Debug function, to be removed

        void					handleDirectory(std::string uri, bool is_directory_listing_enabled);
        void					handleDirectoryDefaultFile(std::string uri);
        void					handleNotFound(std::string uri);

};