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
#include "../webserv.hpp"

class	ConfigParser
{
	public:
		ConfigParser(void);
		~ConfigParser(void);	

		static void	parseConfigs(char *fileName);

	private:
		/*	To be called by parseConfigs()	*/
			//	check syntax in file
		static bool	syntaxCheck(std::string fileName);
			//	load configs into object
		static void	loadConfigs(std::ifstream configFile);
			//	check if configs loaded are OK
		static bool	configsCheck(/* array of configs objects */);
		/*	==============================	*/

		/*	To be called by syntaxCheck()	*/
			//	check if curly brackets in current context are OK
		static bool	contextOK(std::ifstream configFile);
			//	check if current directive is OK
		static bool	directiveOK(std::string configLine);
		/*	==============================	*/

			// to display in case of error in config file
			// initialize to 1 and increment after reading each line
		static int	lineNr;
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