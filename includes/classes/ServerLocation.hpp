#include <vector>
#include "../webserv.hpp"

class	ServerLocation
{
	public:
				ServerLocation(void);
				ServerLocation(std::vector<std::string> strLocationBlock);
		virtual ~ServerLocation(void) = 0;

		std::string	getLocation();
		std::string	getRootDir();
		std::string	getindexDir();
		bool		getAutoIndex();
		
		bool		methodIsAllowed(std::string method);

	protected:											//	KEYWORDS
		std::string					location;			//		location [...] {
		std::string					rootDir;			//		root
		std::string					indexDir;			//		index
		bool						autoIndex;			//		autoindex
		std::vector<Http::METHOD>	methodsAllowed;		//		allowed_methods
		StrStrMap					redirections;		//	
		IntStrMap 					errorPages;			//	
		std::string					uploadDirectory;	//	
};

//	OBJECTS OF THESE CLASSES CAN BE IDENTIFIED WITH DYNAMIC_CAST
class	LocationStatic: public ServerLocation
{
	public:
		LocationStatic();
		~LocationStatic();

};

class	LocationRevProxy: public ServerLocation
{
	public:
		LocationRevProxy();
		~LocationRevProxy();

	private:
		//	proxy_pass

};

class LocationCGI: public ServerLocation
{
	public:
		LocationCGI();
		~LocationCGI();
}