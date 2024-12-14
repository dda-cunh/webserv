#include "../../includes/classes/SessionManager.hpp"

std::vector<Session>	SessionManager::_activeSessions;


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


	cookieCutter(cookieDough, clientCookies);
	for (StrStrMap::const_iterator itt = clientCookies.begin(); itt != clientCookies.end(); itt++)
		_activeSessions.at(i).setCookie(itt->first, itt->second);

	if (!clientCookies.empty() )
		_activeSessions.at(i).setModifiedStatus(true);

	return (_activeSessions.at(i) );		
}

void	SessionManager::cookieCutter(std::string cookieDough, StrStrMap &clientCookies)
{
	std::string	strCookie;
	std::string	cookieKey;
	std::string	cookieVal;


	while (!cookieDough.empty() )
	{
		strCookie = cookieDough.substr(0, cookieDough.find(";") - 1);
		cookieKey = strCookie.substr(0, strCookie.find("=") );
		
		if (cookieKey != "session_id")
		{
			cookieVal = urlDecode(strCookie.substr(strCookie.find("=") + 1, strCookie.npos) );
			clientCookies[cookieKey] = cookieVal;
		}

		cookieDough.erase(0, cookieDough.find(";") + 1);
	}
}

std::string	SessionManager::urlEncode(std::string &value)
{
	std::ostringstream	result;

	for (size_t i = 0; i < value.size(); i++)
	{
		if (std::isalnum(value.at(i) ) \
			|| value.at(i) == '-' || value.at(i) == '_' \
			|| value.at(i) == '.' || value.at(i) == '~')
			result << value.at(i);
		else
			result << "%" << static_cast<int>(value.at(i) );
	}

	return (result.str() );
}

static unsigned char	sHtoC(std::string hexVal)
{
	const char		*vals = "0123456789ABCDEF";
	unsigned char	result = 0;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (hexVal.at(i) == vals[j])
			{
				result += j;
				if (i == 0)
					result = result << 4;
			}
		}
	}
}

std::string	SessionManager::urlDecode(std::string &value)
{
	std::ostringstream	result;

	for (size_t i = 0; i < value.size(); i++)
	{
		if (value.at(i) != '%')
			result << value.at(i);
		else
		{
			result << sHtoC(value.substr(++i, 2) );
			i++;
		}
	}

	return (result.str() );
}