#include <sys/stat.h>
#include <dirent.h>

#include "../../includes/classes/ExceptionMaker.hpp"

#include "../../includes/namespaces/Directory.hpp"

namespace Directory
{
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

	std::map<std::string, std::set<std::string> >	listFilesFull(const std::string &path)
	{
		std::map<std::string, std::set<std::string> >	files;
		dirent *entry;

		DIR *dir = opendir(path.c_str());


		if (!dir)
			throw ExceptionMaker("Failed to open directory: " + path);

		while ( (entry = readdir(dir) ) != NULL)
		{
			if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
				continue ;
			if (entry->d_type == DT_DIR)
				files["FOLDERS"].insert(entry->d_name);
			else
				files["FILES"].insert(entry->d_name);
		}

		closedir(dir);
		return (files);
	}

}