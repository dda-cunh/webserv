#include "../../includes/webserv.hpp"

namespace Http
{
	Http::VERSION	sToVersion(std::string const& v_str)
	{
		if (Utils::lowerStr(v_str) == "http/1.1")
			return (Http::V_1_1);
		return (Http::V_UNHANDLED);
	}

	Http::METHOD	sToMethod(std::string const& m_str)
	{
		if (m_str == "GET")
			return (Http::M_GET);
		else if (m_str == "POST")
			return (Http::M_POST);
		else if (m_str == "DELETE")
			return (Http::M_DELETE);
		return (Http::M_UNHANDLED);
	}

	std::string sToReasonPhrase(Http::STATUS_CODE & statusCode)
	{
		if (statusCode == Http::SC_OK)
			return "OK";
		else if (statusCode == Http::SC_CREATED)
			return "Created";
		else if (statusCode == Http::SC_NO_CONTENT)
			return "No Content";
		else if (statusCode == Http::SC_FOUND)
			return "Found";
		else if (statusCode == Http::SC_BAD_REQUEST)
			return "Bad Request";
		else if (statusCode == Http::SC_FORBIDDEN)
			return "Forbidden";
		else if (statusCode == Http::SC_NOT_FOUND)
			return "Not Found";
		else if (statusCode == Http::SC_METHOD_NOT_ALLOWED)
			return "Method Not Allowed";
		else if (statusCode == Http::SC_CONFLICT)
			return "Conflict";
		else if (statusCode == Http::SC_INTERNAL_SERVER_ERROR)
			return "Internal Server Error";
		else if (statusCode == Http::SC_NOT_IMPLEMENTED)
			return "Not Implemented";
		else if (statusCode == Http::SC_BAD_GATEWAY)
			return "Bad Gateway";
		else if (statusCode == Http::SC_SERVICE_UNAVAILABLE)
			return "Service Unavailable";
		else if (statusCode == Http::SC_VERSION_NOT_SUPPORTED)
			return "HTTP Version Not Supported";
		else
			return "Unknown Status";
	}

	std::string		versionToString(Http::VERSION const& version)
	{
		if (version == Http::V_1_1)
			return ("HTTP/1.1");
		return ("UNHANDLED");
	}

	std::string		methodToString(Http::METHOD const& method)
	{
		if (method == Http::M_GET)
			return ("GET");
		else if (method == Http::M_POST)
			return ("POST");
		else if (method == Http::M_DELETE)
			return ("DELETE");
		return ("UNHANDLED");
	}
}
