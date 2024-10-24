#pragma once

#include "../webserv.hpp"

class CGIMatch {
public:
	CGIMatch() {};
    CGIMatch(const std::string& uri, const std::string& binary);
    void parse(const std::string& uri);
    
    std::string getScriptName() const;
    std::string getScriptExtension() const;
    std::string getBasePath() const;
    std::string getQueryParameters() const;
    std::string getPathInfo() const;
    std::string getCompletePath() const;
	std::string getBinary() const;

    std::string findExtension(const std::string& uri);
    friend std::ostream& operator<<(std::ostream& os, const CGIMatch& cgiMatch);

private:
    std::string _basePath;
    std::string _scriptName;
    std::string _scriptExtension;
    std::string _queryParameters;
    std::string _pathInfo;
    std::string _completePath;
	std::string _binary;

    void extractQueryParameters(const std::string& uri);
};
