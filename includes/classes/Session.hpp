#pragma once

#include "SessionManager.hpp"
#include "../webserv.hpp"

class	SessionManager;


typedef struct	s_cookie
{
	std::string	name;
	std::string	value;	//	HEX VALUE, 32 CHARS
	std::string	expires_maxage;
	std::string	domain;	//	COPY FROM REQUEST URI
	std::string	path;
	bool		secure;	//	ALWAYS SET TO FALSE, WEBSERV DOESN'T SUPPORT HTTPS
	bool		httpOnly;
	std::string	extension;

	//	ADD TIMESTAMP TO BE SET WHEN GENERATING A COOKIE?

}	t_cookie;

t_cookie	& operator = (const t_cookie &lhCookie, const t_cookie &rhCookie);


class	Session
{
	public:

		Session(void);
		Session(const Session &session);
		Session(const Request &request, const ServerConfig &vServer);
		~Session(void);

		Session	& operator = (const Session &session);

		t_cookie					getSessionCookie(void) const;
		std::string					getCookie(std::string var) const;
		StrStrMap::const_iterator	cookiesBegin(void) const;
		StrStrMap::const_iterator	cookiesEnd(void) const;
		bool						sessionStarted(void) const;
		bool						isModified(void) const;

		void						setCookie(std::string key, std::string val);
		void						setModifiedStatus(bool status);


	private:
		t_cookie	_sessionCookie;
		bool		_sessionStarted;
		StrStrMap	_cookies;	//	MAYBE SEPARATE CLIENT FROM SERVER COOKIES?
		bool		_modified;
//	DONT FORGET TO ADD TIMER!!!
};

std::ostream	& operator << (std::ostream &out, const Session &session);
std::ostream	& operator << (std::ostream &out, const t_cookie &cookie);
//	<< overrides for Session class & t_cookie struct