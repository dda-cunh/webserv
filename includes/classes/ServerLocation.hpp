#pragma once

#include "../webserv.hpp"

class	ServerLocation
{
	public:
		ServerLocation(void);
		ServerLocation(const ServerLocation &serverLocation);
		virtual ~ServerLocation(void);

		ServerLocation	&operator = (const ServerLocation &serverLocation);

		std::string					getLocation(void) const;
		std::string					getRootDir(void) const;
		std::string					getIndexFilename(void) const;
		uint32_t					getMaxBodySize(void) const;

		IntStrMap::const_iterator	getErrPageIttBegin(void) const;
		IntStrMap::const_iterator	getErrPageIttEnd(void) const;
		IntStrMap::const_iterator	getRedirectionIttBegin(void) const;
		IntStrMap::const_iterator	getRedirectionIttEnd(void) const;
		Http::METHOD				getMethodByIndex(size_t i) const;
		size_t						getMethodsAllowedSize(void) const;

		std::string					getErrPagePath(int status) const;
		std::string					getRedirection(int status) const;
		bool						methodIsAllowed(Http::METHOD method) const;

	protected:
		std::string					_location;
		std::string					_rootDir;
		std::string					_indexFile;
		uint32_t					_maxBodySize;
		IntStrMap 					_errorPages;
		IntStrMap					_redirections;
		std::vector<Http::METHOD>	_methodsAllowed;
};

class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic(void);
		LocationStatic(const LocationStatic &locationStatic);
		virtual ~LocationStatic(void);

		LocationStatic	&operator = (const LocationStatic &locationStatic);

		bool	getAutoIndex(void) const;

	private:
		bool	_autoIndex;
};

std::ostream & operator << (std::ostream &out, const LocationStatic &locationStatic);
