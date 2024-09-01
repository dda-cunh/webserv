#include "../../includes/webserv.hpp"

namespace ErrorPages
{
	IntStrMap getDefaultErrorPages()
	{
		IntStrMap error_pages;

		error_pages[Http::SC_BAD_REQUEST] = "assets/error_pages/400.html";
		error_pages[Http::SC_FORBIDDEN] = "assets/error_pages/403.html";
		error_pages[Http::SC_NOT_FOUND] = "assets/error_pages/404.html";
		error_pages[Http::SC_METHOD_NOT_ALLOWED] = "assets/error_pages/405.html";
		error_pages[Http::SC_INTERNAL_SERVER_ERROR] = "assets/error_pages/500.html";
		error_pages[Http::SC_NOT_IMPLEMENTED] = "assets/error_pages/501.html";
		return error_pages;
	}
}
