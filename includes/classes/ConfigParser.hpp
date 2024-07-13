/*
	This is a non instatiable class that will read the configuration file
		passed as parameter and check it for syntax errors in the configurations.
	Then it will open the same file again(?) and read it to load the configs
		into memory.
	Finally, it will parse through the loaded configurations and check if they
		are valid for the program to run.


	HMM... MAYBE CREATE A CHECKER OBJECT THAT USES FUNCTIONS FROM PARSER TO CHECK CONFIG FILE
*/

#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include "../webserv.hpp"

//	DEFINE STRING ARRAYS FOR KEYWORDS SO THE PARSER CAN VERIFY WITH SWITCH/CASE
# define SERVER_KEYWORDS {"listen", "server_name", "root", "client_max_body_size", "error_page", NULL}
# define LOCATION_KEYWORDS {"root", "index", "autoindex", "allow_methods", "proxy_pass" /* and whatever will be used for CGI */, NULL} 

typedef std::vector<ServerConfig>		ServerBlocks;
typedef std::vector<AServerLocation*>	LocationBlocks;

//	OPEN THE FILE AND WRITE IT ALL AT ONCE INTO MEMORY FOR EASIER INDEXING
//	INCLUDE SAFEGUARDS FOR FILES TOO LARGE
//	IF FILE IS EMPTY, CAN EITHER THROW AN EXCEPTION OR SET DEFAULT VALUES
class	ConfigParser
{
	public:

		static void	parseConfigs(std::string fileName/*, ServerBlocks &serverConfigs*/);	//	UNCOMMENT

		/*	To be called by parseConfigs()	*/
			//	check if current server context is OK and load it to memory
		static bool	serverBlockOK(std::ifstream &configFile);
			//	load configs into object
//		static void	loadConfigs(std::string fileName, &serverBlocks); 
			//	create a new server block
//		static bool	addServerBlock(std::vector<std::string> strServerBlock);
			//	check if configs loaded are OK
//		static bool	configsCheck(ServerBlocks &serverBlocks);
		/*	==============================	*/

		/*	To be called by serverBlockOK()	*/
		static void	trimConfigLine(std::string &configLine);	//	will also be called by copyToVector()
		static bool	serverBlockHeaderOK(std::string configLine);
		static bool	copyToVector(std::ifstream &configFile);
		static bool	syntaxCheck(char *keywords);	//	HUH... MAYBE WE'LL NEED TO USE A FUNCTION FOR EACH BLOCK TYPE?
		/*	==============================	*/

		/*	To be called by serverBlockHeaderOK()	*/
		static unsigned int		wordCount(std::string configLine);
		/*	==============================	*/

		/*	To be called by syntaxCheck()	*/
			//	check if current location context is OK
//		static bool	locationContextOK(std::ifstream configFile);
			//	check if current directive is OK - WILL BE CALLED BY THE 2 METHODS ABOVE
//		static bool	directiveOK(std::string configLine);
		/*	==============================	*/


		static std::vector<std::string>	_strServerBlock;
		static std::vector<std::string>	_strLocationBlock;

	private:
		ConfigParser(void);
		~ConfigParser(void);



		//	DON'T FORGET DO ADD EXCEPTIONS


		/*	prob not gonna use these	*/
//		static ServerConfig		&loadServerConfig(/* struct goes here */, ServerBlocks &serverBlocks);
//		static ServerLocation	&loadServerLocation(/* struct goes here */, LocationBlocks &locationBlocks);
		/*	==============================	*/


};


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
			- whatever we'll use to handle CGI; check with the team
*/