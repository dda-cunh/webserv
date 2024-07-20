#pragma once

#include "../webserv.hpp"

namespace ErrorPages
{

	//TODO: this typedef is already declared elsewhere, fix to avoid repetition
	typedef std::map<int, std::string>				IntStrMap;

	IntStrMap		getDefaultErrorPages();
}