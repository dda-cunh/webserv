#include "../../includes/classes/Session.hpp"

Session::t_cookie	&operator=(const struct s_cookie &cookie)
{
	this->value = cookie.value;
	//	expires
	//	max_age
	//	domain
	path = "/";
	this->secure = cookie.secure;
	this->httpOnly = cookie.httpOnly;
	this->extension = cookie.extension;

	return (this);
}

//	HUH... MAYBE THIS WON'T BE NEEDED ATER ALL?
Session::Session(void)
{
	//	GENERATE SESSION ID COOKIE
	this->_sessionStarted = false;	//	SET TO TRUE AFTER SENDING COOKIE
}

Session::Session(const Session &session)
{
	if (this != &session)
		*this = session;
}

static std::string	gen_session_id(void)
{
	std::string 	result;
	//	CLOCK TIME + RANDOM VAL
	//	32 CHARS, HEX

	return (result);
}

static std::string	set_cookie_domain(ServerConfig &vServer, std::string &requestURI)
{

}

Session::Session(const Request &request, const ServerConfig &vServer)
{
	//	GENERATE SESSION ID COOKIE FROM REQUEST
	this->_sessionCookie.name = "SessionID";
	this->_sessionCookie.value = gen_session_id();
	this->_sessionCookie.expires_maxage = "Session";
	this->_sessionCookie.domain = set_cookie_domain(vServer, request.uri() );
	this->_sessionCookie.path = "/";
	this->_sessionCookie.secure = false;
	this->_sessionCookie.httpOnly = true;

	this->_sessionStarted = false;	//	SET TO TRUE AFTER SENDING COOKIE
	this->_modified = true;			//	IS THIS REALLY NEEDED?

	//	SEARCH FOR OTHER COOKIES IN REQUEST HEADER & ADD TO _cookies
}

Session	&Session::operator=(const Session &session)
{
	this->_cookies.clear();

	for (std::map<std::string, t_cookie>::const_iterator itt = session.cookiesBegin(); itt != session.cookiesEnd(); itt++)
		this->_cookies[itt->first] = itt->second;

	return (*this);
}

Session::~Session(void)
{
	return ;
}

Session::t_cookie	Session::getSessionCookie(void) const
{
	return (this->_sessionCookie);
}

std::string	Session::getCookie(std::string var) const
{
	return (this->_cookies.at(var) );
}

StrStrMap::const_iterator	Session::cookiesBegin(void) const
{
	return (this->_cookies.begin() );
}

StrStrMap::const_iterator	Session::cookiesEnd(void) const
{
	return (this->_cookies.end() );
}

void	Session::setCookie(std::string key, std::string val)
{
	this->_cookies[key] = SessionManager::urlEncode(val);
}

void	Session::parseClientCookies(std::string clientCookies)
{

}