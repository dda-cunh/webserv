#pragma once

#include <string>
#include <dirent.h>

#include "../webserv.hpp"

namespace Directory
{
	struct Result
	{
		Http::STATUS_CODE	statusCode;
		std::string			path;
	};

	Result	handleDirectory(const std::string path, const bool autoindex);
	Result	listDirectory(const std::string &path);
	Result	handleDirectoryDefaultFile(const std::string path);
}
