#include "../../includes/classes/SessionManager.hpp"

std::vector<Session>	SessionManager::_activeSessions;

static void	cookie_cutter(std::string &cookieDough, StrStrMap &clientCookies)
{
	//	DONT FORGET TO EXCLUDE SESSION COOKIE!!!
}

//	PROB GONNA HAVE TO FIX SOME OF THIS LOGIC TO ACCOMODATE HOST CHECK
Session	&SessionManager::getCurrentSession(std::string cookieDough, ServerConfig &vServer)
{
	Session		newSession(trigData->req, vServer);
	std::string	strSessionID;
	StrStrMap	clientCookies;


	if (cookieDough == "")
	{
		_activeSessions.push_back(newSession);
		return (_activeSessions.at(_activeSessions.size() - 1) );
	}

	//	GET session_id COOKIE
	strSessionID = cookieDough.substr(cookieDough.find("session_id="), cookieDough.npos);
	strSessionID.erase(strSessionID.find(";"), strSessionID.npos);

	if (strSessionID == "")
	{
		_activeSessions.push_back(newSession);
		return (_activeSessions.at(_activeSessions.size() - 1) );
	}


	size_t i = 0;
	while (i < _activeSessions.size() )
	{
		if (_activeSessions.at(i).getCookie("session_id").value == strSessionID)
		{
			newSession = _activeSessions.at(i);
			break ;
		}

		i++;
	}
	if (i == _activeSessions.size() )
		_activeSessions.push_back(newSession);


	//	PARSE COOKIES FROM cookieDough AND ASSIGN THEM TO THE SESSION COOKIES
	//	DO THIS IN ANOTHER FUNCTION
	cookie_cutter(cookieDough, clientCookies);
	for (StrStrMap::const_iterator itt = clientCookies.begin(); itt != clientCookies.end(); itt++)
		_activeSessions.at(i).setCookie(itt->first, itt->second);

	if (!clientCookies.empty() )
		_activeSessions.at(i).setModifiedStatus(true);

	return (_activeSessions.at(i) );		
}
