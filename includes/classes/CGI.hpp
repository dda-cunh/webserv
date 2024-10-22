#pragma once

#include "../webserv.hpp"

class CGI {
public:
	CGI() {};
    CGI(const std::string& uri, const std::string& binary);
    void parse(const std::string& uri);
    
    std::string getScriptName() const;
    std::string getScriptExtension() const;
    std::string getCompletePath() const;
    std::string getBasePath() const;
    std::string getQueryParameters() const;
	std::string getBinary() const;

    friend std::ostream& operator<<(std::ostream& os, const CGI& cgi);

private:
    std::string _basePath;
    std::string _scriptName;
    std::string _scriptExtension;
    std::string _completePath;
    std::string _queryParameters;
	std::string _binary;

    void extractQueryParameters(const std::string& uri);
};
