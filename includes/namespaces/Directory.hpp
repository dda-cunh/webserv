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

	Result											handleDirectory(const std::string path, const bool autoindex);
	Result											listDirectory(const std::string &path);
	Result											handleDirectoryDefaultFile(const std::string path);
					
	bool											isDirectory(const std::string &path);
	std::vector<std::string>						listFiles(const std::string &path);
	std::map<std::string, std::set<std::string> >	listFilesFull(const std::string &path);
}
