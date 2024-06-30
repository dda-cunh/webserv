#pragma once

#define CRLF	"\r\n"

#include <string>

namespace Http
{
	typedef enum	e_version
	{
		V_UNHANDLED,
		V_1_1
	}	VERSION;

	typedef enum	e_method
	{
		M_UNHANDLED,
		M_DELETE,
		M_POST,
		M_GET,
		__LEN_METHOD
	}	METHOD;

	/****************************  RESPONSE CODES  ****************************/
	typedef enum	e_status_code
	{
		SC_OK = 200,
		SC_CREATED = 201,
		SC_NO_CONTENT = 204, //The most common success code for DELETE
		SC_BAD_REQUEST = 400,
		SC_FORBIDDEN = 403, //Getting a resource
		SC_NOT_FOUND = 404,
		SC_METHOD_NOT_ALLOWED = 405, //If no permissions for method in path
		SC_CONFLICT = 409, //If the data to post already exist and shouldnt be overwritten
		SC_INTERNAL_SERVER_ERROR = 500,
		SC_NOT_IMPLEMENTED = 501,
		SC_BAD_GATEWAY = 502,
		SC_SERVICE_UNAVAILABLE = 503,
		SC_VERSION_NOT_SUPPORTED = 505
	}	STATUS_CODE;

	Http::VERSION	sToVersion(std::string const&);
	Http::METHOD	sToMethod(std::string const&);
	std::string		versionToString(Http::VERSION const&);
	std::string		methodToString(Http::METHOD const&);
	/**************************************************************************/
}
