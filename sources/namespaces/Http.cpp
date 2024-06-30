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
