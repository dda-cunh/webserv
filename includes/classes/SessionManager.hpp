#pragma once

#include "Session.hpp"


class	SessionManager
{
	public:
		static Session		&getCurrentSession(std::string clientCookies, ServerConfig &vServer);
		static void			cookieCutter(std::string cookieDough, StrStrMap &clientCookies);
		static std::string	urlEncode(std::string &value);
		static std::string	urlDecode(std::string &value);
		//	static void saveToDisk(Session &session);
		//	static void	loadFromDisk(std::string sessionID);

	private:	
		SessionManager();
		~SessionManager();

		static std::vector<Session>	_activeSessions;	
};