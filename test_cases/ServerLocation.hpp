#include <vector>
#include <map>
#include <iostream>
#include "ServerConfig.hpp"

typedef std::map<std::string, std::string>		StrStrMap;
typedef std::map<int, std::string>				IntStrMap;


class	ServerLocation
{
	public:
		ServerLocation(void);
//		ServerLocation(std::vector<std::string> strLocationBlock);
		ServerLocation(const ServerLocation &serverLocation);
		virtual ~ServerLocation(void) = 0;

		ServerLocation	&operator = (const ServerLocation &serverLocation);

		std::string			getLocation(void) const;
		std::string			getRootDir(void) const;
		std::string			getIndexFilename(void) const;
		uint32_t			getMaxBodySize(void) const; const

		IntStrMap::iterator	getErrPageIttBegin(void) const;
		IntStrMap::iterator	getErrPageIttEnd(void) const;
		IntStrMap::iterator	getRedirectionIttBegin(void) const;
		IntStrMap::iterator	getRedirectionIttEnd(void) const;
		http::METHOD		getMethodByIndex(size_t i) const;
		size_t				getMethodsAllowedSize(void) const; const

		std::string			getErrPagePath(int status) const;
		std::string			getRedirection(int status) const;
		bool				methodIsAllowed(Http::METHOD method) const;

	protected:											//	KEYWORDS
		std::string					_location;			//		location [...] {
		std::string					_rootDir;			//		root
		std::string					_indexFile;			//		index
		uint32_t					_maxBodySize;		//		client_max_body_size; default is 1m
		IntStrMap 					_errorPages;		//		error_page
		IntStrMap					_redirections;		//		return
		std::vector<Http::METHOD>	_methodsAllowed;	//		allowed_methods
};

//	OBJECTS OF THESE CLASSES CAN BE IDENTIFIED WITH DYNAMIC_CAST
class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic(void);
//		LocationStatic(std::vector<std::string> strLocationBlock);
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