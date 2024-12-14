#pragma once

#include "Session.hpp"
#include "../webserv.hpp"

class	Session;

class	SessionManager
{
	public:
		static Session		&getCurrentSession(Request &request, std::string clientCookies, ServerConfig &vServer);
		static void			cookieCutter(std::string cookieDough, StrStrMap &clientCookies);
		static std::string	urlEncode(const std::string value);
		static std::string	urlDecode(const std::string value);
		//	static void saveToDisk(Session &session);
		//	static void	loadFromDisk(std::string sessionID);

	private:	
		SessionManager();
		~SessionManager();

		static std::vector<Session>	_activeSessions;	
};