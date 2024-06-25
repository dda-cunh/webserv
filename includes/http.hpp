#pragma once

#define CRLF	"\r\n"

typedef enum	e_HTTP_VERSION
{
	HTTP_V_UNHANDLED,
	HTTP_V_1_1
}	HTTP_VERSION;

typedef enum	e_method
{
	HTTP_M_UNHANDLED,
	HTTP_M_DELETE,
	HTTP_M_POST,
	HTTP_M_GET,
	__LEN_HTTP_METHOD
}	HTTP_METHOD;

/****************************  RESPONSE CODES  ****************************/
typedef enum	e_http_status_codes
{
	HTTP_SC_OK = 200,
	HTTP_SC_CREATED = 201,
	HTTP_SC_NO_CONTENT = 204, //The most common success code for DELETE
	HTTP_SC_BAD_REQUEST = 400,
	HTTP_SC_FORBIDDEN = 403, //Getting a resource
	HTTP_SC_NOT_FOUND = 404,
	HTTP_SC_METHOD_NOT_ALLOWED = 405, //If no permissions for method in path
	HTTP_SC_CONFLICT = 409, //If the data to post already exist and shouldnt be overwritten
	HTTP_SC_INTERNAL_SERVER_ERROR = 500,
	HTTP_SC_NOT_IMPLEMENTED = 501,
	HTTP_SC_BAD_GATEWAY = 502,
	HTTP_SC_SERVICE_UNAVAILABLE = 503,
	HTTP_SC_HTTP_VERSION_NOT_SUPPORTED = 505
}	HTTP_STATUS_CODE;
/**************************************************************************/
