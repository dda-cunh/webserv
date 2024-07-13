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
		dirent *ent;
		std::string htmlFilePath = path + "/directory_listing.html";
		std::ofstream htmlFile(htmlFilePath.c_str());
		std::string root = "test_files/www/"; // TODO: get this from config
		Result result;

		result.path = "";
		if (!htmlFile)
		{
			result.statusCode = Http::SC_INTERNAL_SERVER_ERROR;
			return result;
		}

		htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Listing</title>\n \
					</head>\n<body>\n<h1>Directory Listing for "
				 << path.substr(root.length()) << "</h1>\n<ul>\n";
		if ((dir = opendir(path.c_str())))
		{
			while ((ent = readdir(dir)))
				if (ent->d_name[0] != '.' && strcmp(ent->d_name, "directory_listing.html") != 0)
					htmlFile << "<li>" << ent->d_name << "</li>\n";
			closedir(dir);

			htmlFile << "</ul>\n</body>\n</html>";
			result.statusCode = Http::SC_OK;
			result.path = htmlFilePath;
		}
		else
		{
			result.statusCode = Http::SC_INTERNAL_SERVER_ERROR;
		}
		htmlFile.close();
		return result;
	}

	Result handleDirectoryDefaultFile(const std::string path)
	{
		Result result;
		std::vector<std::string> default_filenames;
		default_filenames.push_back("index.html"); // TODO: get this from config
		for (std::vector<std::string>::iterator it = default_filenames.begin(); it != default_filenames.end(); ++it)
		{
			if (Utils::resourceExists(path + "/" + *it))
			{
				result.statusCode = Http::SC_OK;
				result.path = path + "/" + *it;
				return result;
			}
		}
		result.statusCode = Http::SC_NOT_FOUND;
		result.path = path;
		return result;
	}
}