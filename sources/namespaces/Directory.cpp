#include "../../includes/webserv.hpp"

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

//	COPY/PASTE listFilesFull from vservers branch

}