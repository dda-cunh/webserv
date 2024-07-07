/*
	This is a non instatiable class that will read the configuration file
		passed as parameter and check it for syntax errors in the configurations.
	Then it will open the same file again(?) and read it to load the configs
		into memory.
	Finally, it will parse through the loaded configurations and check if they
		are valid for the program to run.
*/

#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include "../webserv.hpp"

//	DEFINE STRING ARRAYS FOR KEYWORDS SO THE PARSER CAN VERIFY WITH SWITCH/CASE
# define SERVER_KEYWORDS {"listen", "server_name", "root", "client_max_body_size", "error_page"}
# define LOCATION_KEYWORDS {"root", "index", "autoindex", "allow_methods", "proxy_pass" /* and whatever will be used for CGI */} 

typedef std::vector<ServerConfig>	ServerBlocks;
typedef std::vector<AServerLocation*>	LocationBlocks;

//	OPEN THE FILE AND WRITE IT ALL AT ONCE INTO MEMORY FOR EASIER INDEXING
//	INCLUDE SAFEGUARDS FOR FILES TOO LARGE
//	IF FILE IS EMPTY, CAN EITHER THROW AN EXCEPTION OR SET DEFAULT VALUES
class	ConfigParser
{
	public:

		static void	parseConfigs(std::string *fileName);

	private:
		ConfigParser(void);
		~ConfigParser(void);	

		/*	To be called by parseConfigs()	*/
			//	check if current server context is OK and load it to memory
		static bool	serverContextOK(std::ifstream configFile);
			//	create a new server block
		static bool	addServerBlock(std::vector<std::string> strServerBlock);
			//	check syntax in block
		static bool	syntaxCheck(std::vector<std::string> strServerBlock);
			//	load configs into object
		static void	loadConfigs(std::string fileName, &serverBlocks); 
			//	check if configs loaded are OK
		static bool	configsCheck(&serverBlocks);
		/*	==============================	*/

		/*	To be called by syntaxCheck()	*/
			//	check if current location context is OK
		static bool	locationContextOK(std::ifstream configFile);
			//	check if current directive is OK - WILL BE CALLED BY THE 2 METHODS ABOVE
		static bool	directiveOK(std::string configLine);
		/*	==============================	*/

		/*	To be called by loadConfigs()	*/	/*|	is this copying right?|	*/
		static ServerConfig		&loadServerConfig(std::ifstream configFile, ServerBlocks &serverBlocks);
		static ServerLocation	&loadServerLocation(std::ifstream configFile, LocationBlocks &locationBlocks);
		/*	==============================	*/

			// to display in case of error in config file
			// initialize to 1 and increment after reading each line
		static unsigned int		_lineNr;
		static std::vector<std::string>	_strServerBlock;
		static std::vector<std::string>	_locationRaw;

		//	DON'T FORGET DO ADD EXCEPTIONS
};

//	THESE ARE UTIL FUNCTIONS; MOVE TO UTILS.HPP BEFORE MERGE
int	wordCount(std::string configLine);

/*
	keywords to accept for directives in each context
	> server
		listen
		server_name
		root
		client_max_body_size
		error_page

		> location
			root
			index
			autoindex
			allow_methods
			proxy_pass
			- how are we gonna handle CGI?
*/