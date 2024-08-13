#include <vector>
#include "../webserv.hpp"

class	ServerLocation
{
	public:
		ServerLocation(void);
		ServerLocation(std::vector<std::string> strLocationBlock);
		ServerLocation(const ServerLocation &serverLocation);
		virtual ~ServerLocation(void) = 0;

		ServerLocation	&operator = (const ServerLocation &serverLocation);

		std::string	getLocation(void);
		std::string	getRootDir(void);
		std::string	getIndexFilename(void);
		uint32_t	getMaxBodySize(void);
		std::string	getErrPagePath(int status);
		std::string	getRedirection(int status); // give this parameter an appropriate name...
		bool		methodIsAllowed(Http::METHOD method);

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
		LocationStatic(std::vector<std::string> strLocationBlock);
		LocationStatic(const LocationStatic &locationStatic);
		~LocationStatic(void);

		LocationStatic	&operator = (const LocationStatic &locationStatic);

		bool	getAutoIndex(void);

	private:
		bool	_autoIndex;			//		autoindex
		//	try_files
};

//	THIS WILL BE FOR FILE UPLOADS
//	huh... fuck, now I'm not sure if this still needs to be a reverse proxy...
class	LocationRevProxy: public ServerLocation
{
	public:
		LocationRevProxy(void);
		LocationRevProxy(std::vector<std::string> strLocationBlock);
		~LocationRevProxy(void);

		std::string	getUploadDir(void);

	private:
		//	proxy_pass
		std::string	_uploadDirectory;	//	THIS WILL REQUIRE ITS OWN LOCATION BLOCK...

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
		//	proxy_pass ?
		//	fastcgi_pass
		//	fastcgi_param
		//	fastcgi_index
		//	fastcgi_split_path_info
}

//	<< overloads
std::ostream & operator << (std::ostream &out, const ServerLocation &serverLocation);
std::ostream & operator << (std::ostream &out, const LocationStatic &locationStatic);
//	overload for file uploads location block
std::ostream & operator << (std::ostream &out, const LocationCGI &locationCGI);