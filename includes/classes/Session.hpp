#pragma once

#include "ServerManager.hpp"


class	Session
{
	public:

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

		t_cookie	& operator = (const t_cookie &cookie);


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

		void						parseClientCookies(std::string clientCookies);


	private:
		t_cookie	_sessionCookie;
		bool		_sessionStarted;
		StrStrMap	_cookies;	//	MAYBE SEPARATE CLIENT FROM SERVER COOKIES?
		bool		_modified;	//	TO CHECK WHETHER SESSION DATA IN PERSISTENT STORAGE NEEDS TO BE UPDATED
//	DONT FORGET TO ADD TIMER!!!
};

std::ostream	& operator << (std::ostream &out, const Session &session);
std::ostream	& operator << (std::ostream &out, const t_cookie &cookie);
//	<< overrides for Session class & t_cookie struct