#include "../../includes/namespaces/ErrorPages.hpp"
#include "../../includes/namespaces/Http.hpp"

namespace ErrorPages
{
	IntStrMap getDefaultErrorPages()
	{
		IntStrMap error_pages;

		error_pages[Http::SC_BAD_REQUEST] = "web/assets/error_pages/400.html";
		error_pages[Http::SC_FORBIDDEN] = "web/assets/error_pages/403.html";
		error_pages[Http::SC_NOT_FOUND] = "web/assets/error_pages/404.html";
		error_pages[Http::SC_METHOD_NOT_ALLOWED] = "web/assets/error_pages/405.html";
		error_pages[Http::SC_INTERNAL_SERVER_ERROR] = "web/assets/error_pages/500.html";
		error_pages[Http::SC_NOT_IMPLEMENTED] = "web/assets/error_pages/501.html";
		return error_pages;
	}
}
