#include "../../includes/webserv.hpp"

namespace Directory
{
	Result handleDirectory(const std::string path, const bool autoindex)
	{
		if (autoindex && !path.empty() && path[path.length() - 1] == '/')
			return listDirectory(path);
		else
			return handleDirectoryDefaultFile(path);
	}

	Result listDirectory(const std::string &path)
	{
		DIR *dir;
		struct dirent *ent;
		std::string htmlFilePath = path + "/directory_listing.html";
		std::ofstream htmlFile(htmlFilePath.c_str());
		std::string root = "test_files/www/"; // TODO: get this from config
		Result result;

		if (!htmlFile.is_open())
		{
			result.statusCode = Http::SC_INTERNAL_SERVER_ERROR;
			result.path = htmlFilePath;
			return result;
		}

		htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Listing</title>\n \
					</head>\n<body>\n<h1>Directory Listing for "
				 << path.substr(root.length()) << "</h1>\n<ul>\n";
		if ((dir = opendir(path.c_str())) != NULL)
		{
			while ((ent = readdir(dir)) != NULL)
				if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 && strcmp(ent->d_name, "directory_listing.html") != 0)
					htmlFile << "<li>" << ent->d_name << "</li>\n";
			closedir(dir);

			htmlFile << "</ul>\n</body>\n</html>";

			result.statusCode = Http::SC_OK;
			result.path = htmlFilePath;
		}
		else
		{
			result.statusCode = Http::SC_INTERNAL_SERVER_ERROR;
			result.path = htmlFilePath;
		}
		htmlFile.close();
		return result;
	}

	Result handleDirectoryDefaultFile(const std::string path)
	{
		Result result;
		std::string default_filenames[] = {"index.html"}; // TODO: get this from config
		for (size_t i = 0; i < sizeof(default_filenames) / sizeof(default_filenames[0]); i++)
		{
			if (Utils::resourceExists(path + "/" + default_filenames[i]))
			{
				result.statusCode = Http::SC_OK;
				result.path = path + "/" + default_filenames[i];
				return result;
			}
		}
		result.statusCode = Http::SC_NOT_FOUND;
		result.path = path;
		return result;
	}
}