#pragma once

#include "Session.hpp"


class	SessionManager
{
	public:
		static Session	&getCurrentSession(std::string clientCookies, ServerConfig &vServer);

		//	static void saveToDisk(Session &session);
		//	static void	loadFromDisk(std::string sessionID);

	private:	
		SessionManager();
		~SessionManager();

		std::vector<Session>	_activeSessions;	
};