#pragma once

#include "../webserv.hpp"
#include "ConfigParser.hpp"


//	DEFAULTS
#define DEFAULT_LOCATION "/"
#define DEFAULT_ROOT "./test_files"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_MAX_BODY_SIZE 1000000

#define DEFAULT_400 "./assets/error_pages/400.html"
#define DEFAULT_403 "./assets/error_pages/403.html"
#define DEFAULT_404 "./assets/error_pages/404.html"
#define DEFAULT_405 "./assets/error_pages/405.html"
#define DEFAULT_500 "./assets/error_pages/500.html"
#define DEFAULT_501 "./assets/error_pages/501.html"

#define DEFAULT_AUTO_INDEX false


class	ServerLocation
{
	public:
		ServerLocation(void);
		ServerLocation(std::vector<std::string> strLocationBlock);
		ServerLocation(const ServerLocation &serverLocation);
		virtual ~ServerLocation(void);

		ServerLocation	&operator = (const ServerLocation &serverLocation);

		std::string					getLocation(void) const;
		std::string					getRootDir(void) const;
		std::string					getIndexFileName(size_t i) const;
		uint32_t					getMaxBodySize(void) const;
		std::string					getErrPagePath(int status) const;
		std::string					getRedirection(std::string url) const;
		bool						methodIsAllowed(Http::METHOD method) const;
		std::string					getUploadPath(void) const;

		IntStrMap::const_iterator	getErrPageIttBegin(void) const;
		IntStrMap::const_iterator	getErrPageIttEnd(void) const;
		StrStrMap::const_iterator	getRedirectionIttBegin(void) const;
		StrStrMap::const_iterator	getRedirectionIttEnd(void) const;
		Http::METHOD				getMethodByIndex(size_t i) const;
		size_t						getMethodsAllowedSize(void) const;
		size_t						getIndexVectorSize(void) const;


	protected:											//	KEYWORDS

		std::string					_location;			//		location [PATH] {
		std::string					_rootDir;			//		root [PATH]
		std::vector<std::string>	_indexFiles;		//		index FILE0 [FILE 1..N]
		uint32_t					_maxBodySize;		//		client_max_body_size [UINT]
		IntStrMap 					_errorPages;		//		error_page [STATUS] [PATH]
		StrStrMap					_redirections;		//		rewrite [URI | SERVERNAME] [URI | SERVERNAME]
		std::vector<Http::METHOD>	_methodsAllowed;	//		allow_methods [GET] [POST] [DELETE]
		std::string					_uploadPath;		//		upload_store [PATH]


	private:
		std::string					_setLocation(std::string locationLine);
		std::string					_setRootDir(std::vector<std::string> strLocationBlock);
		void						_setIndexFiles(std::vector<std::string> strLocationBlock, std::vector<std::string> &indexFiles);
		uint32_t					_setMaxBodySize(std::vector<std::string> strLocationBlock);
		void						_setErrorPages(std::vector<std::string> strLocationBlock, IntStrMap &errorPages);
		void						_setRedirections(std::vector<std::string> strLocationBlock, StrStrMap &redirections);
		void						_setAllowedMethods(std::vector<std::string> strLocationBlock, std::vector<Http::METHOD> &methodsAllowed);
		std::string					_setUploadStore(std::vector<std::string> strLocationBlock);
};


class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic(void);
		LocationStatic(std::vector<std::string> strLocationBlock);
		LocationStatic(const LocationStatic &locationStatic);
		virtual ~LocationStatic(void);

		LocationStatic	&operator = (const LocationStatic &locationStatic);

		bool	getAutoIndex(void) const;

		bool	_setAutoIndex(std::vector<std::string> strLocationBlock);

	private:
		bool	_autoIndex;			//		autoindex [on | off]
};


/*
class LocationCGI: public ServerLocation
{
	public:
		LocationCGI(void);
		LocationCGI(std::vector<std::string> strLocationBlock);
		LocationCGI(const LocationCGI &locationCGI);
		~LocationCGI(void);

		LocationCGI	&operator = (const LocationCGI & locationCGI);

	private:
		StrStrMap	_cgiPass	//	cgi_pass [EXT] [PATH];
}
*/

std::ostream & operator << (std::ostream &out, const LocationStatic &locationStatic);

//std::ostream & operator << (std::ostream &out, const LocationCGI &locationCGI);
