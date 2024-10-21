#pragma once

class	CGIParser
{
	public:
		static std::string	parseURI(const Request &request, const ServerLocation *serverLocation);
		static std::string	parseCgiPath(const Request &request, const ServerLocation *serverLocation)
		//	ALSO PARSE ARGS

	private:
		CGIParser(void);
		~CGIParser(void);
};
