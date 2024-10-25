#include "../../includes/webserv.hpp"

namespace Directory
{
	Result handleDirectory(const std::string path, const bool autoindex)
	{
		//	FIRST CHECK IF INDEX FILES EXIST, THEN CHECK IF autoindex IS ON

		std::string root = "public/";
		if (autoindex && !path.empty() && path[path.length() - 1] == '/' && path != root)
			return listDirectory(path);
		else
			return handleDirectoryDefaultFile(path);
	}

	Result listDirectory(const std::string &path)
	{
		std::string htmlFilePath = path + "/directory_listing.html";
		std::ofstream htmlFile(htmlFilePath.c_str());
		std::string root = "public/";
		Result result;

		result.path = "";
		if (!htmlFile)
		{
			result.statusCode = Http::SC_INTERNAL_SERVER_ERROR;
			return result;
		}

		try
		{
			htmlFile << "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Listing</title>\n \
                </head>\n<body>\n<h1>Directory Listing for "
					 << path.substr(root.length()) << "</h1>\n<ul>\n";

			std::vector<std::string> files = listFiles(path);
			for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
			{
				if (*it != "directory_listing.html")
				{
					htmlFile << "<li>" << *it << "</li>\n";
				}
			}

			htmlFile << "</ul>\n</body>\n</html>";
			result.statusCode = Http::SC_OK;
			result.path = htmlFilePath;
		}
		catch (ExceptionMaker &e)
		{
			e.log();
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

	bool isDirectory(const std::string &path)
	{
		struct stat statbuf;

		if (stat(path.c_str(), &statbuf) != 0)
			return false;
		return S_ISDIR(statbuf.st_mode);
	}

	std::vector<std::string> listFiles(const std::string &directory)
	{
		std::vector<std::string> files;
		DIR *dir = opendir(directory.c_str());
		if (!dir)
			throw ExceptionMaker("Failed to open directory: " + directory);

		dirent *entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_REG)
				files.push_back(entry->d_name);
		}
		closedir(dir);
		return files;
	}
}