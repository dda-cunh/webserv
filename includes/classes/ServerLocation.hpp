#pragma once

#include "../webserv.hpp"
#include "ConfigParser.hpp"


//	DEFAULTS
#define DEFAULT_LOCATION "/"
#define DEFAULT_ROOT "./test_files"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_MAX_BODY_SIZE 1000000

#define DEFAULT_404 "./test_files/error_pages/404.html"
#define DEFAULT_405 "./test_files/error_pages/405.html"
#define DEFAULT_500 "./test_files/error_pages/500.html"

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

		IntStrMap::const_iterator	getErrPageIttBegin(void) const;
		IntStrMap::const_iterator	getErrPageIttEnd(void) const;
		IntStrMap::const_iterator	getRedirectionIttBegin(void) const;
		IntStrMap::const_iterator	getRedirectionIttEnd(void) const;
		Http::METHOD				getMethodByIndex(size_t i) const;
		size_t						getMethodsAllowedSize(void) const;

		std::string					getErrPagePath(int status) const;
		std::string					getRedirection(int status) const;
		bool						methodIsAllowed(Http::METHOD method) const;

		size_t						getIndexVectorSize(void) const;

	protected:											//	KEYWORDS
		std::string					_location;			//		location [...] {
		std::string					_rootDir;			//		root
		std::vector<std::string>	_indexFiles;		//		index
		uint32_t					_maxBodySize;		//		client_max_body_size
		IntStrMap 					_errorPages;		//		error_page
		IntStrMap					_redirections;		//		return
		std::vector<Http::METHOD>	_methodsAllowed;	//		allow_methods
};

//	OBJECTS OF THESE CLASSES CAN BE IDENTIFIED WITH DYNAMIC_CAST
class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic(void);
		LocationStatic(std::vector<std::string> strLocationBlock);
		LocationStatic(const LocationStatic &locationStatic);
		virtual ~LocationStatic(void);

		LocationStatic	&operator = (const LocationStatic &locationStatic);

		bool	getAutoIndex(void) const;

	private:
		bool	_autoIndex;			//		autoindex
		//	try_files
};

//	NO NEED FOR A REVERSE PROXY; EITHER IMPLEMENT A CLASS JUST FOR UPLOADS
//		OR INCORPORATE IT IN LOCATIONSTATIC
/*
class	LocationRevProxy: public ServerLocation
{
	public:
		LocationRevProxy(void);
		LocationRevProxy(std::vector<std::string> strLocationBlock);
		~LocationRevProxy(void);

		std::string	getUploadDir(void);

	private:

		std::string	_uploadDirectory;

};

class LocationCGI: public ServerLocation
{
	public:
		LocationCGI(void);
		LocationCGI(std::vector<std::string> strLocationBlock);
		LocationCGI(const LocationCGI &locationCGI);
		~LocationCGI(void);

		LocationCGI	&operator = (const LocationCGI & locationCGI);

	private:
		//	fastcgi_pass
		//	fastcgi_param
		//	fastcgi_index
		//	fastcgi_split_path_info
}
*/

std::ostream & operator << (std::ostream &out, const LocationStatic &locationStatic);

//std::ostream & operator << (std::ostream &out, const LocationCGI &locationCGI);
