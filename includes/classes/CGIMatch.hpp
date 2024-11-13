#pragma once

#include "../webserv.hpp"

class CGIMatch {
public:
    CGIMatch() {};
    CGIMatch(const std::string& uri, const std::string& binary);
    CGIMatch(const CGIMatch& other);
    CGIMatch& operator=(const CGIMatch& other);
    
    void parse(const std::string& uri);
    std::string getScriptName() const;
    std::string getScriptExtension() const;
    std::string getBasePath() const;
    std::string getPathInfo() const;
    std::string getCompletePath() const;
    std::string getBinary() const;
    std::string getQueryString() const;
    std::string findExtension(const std::string& uri);
    
    friend std::ostream& operator<<(std::ostream& os, const CGIMatch& cgiMatch);

private:
    std::string _basePath;
    std::string _scriptName;
    std::string _scriptExtension;
    std::string _queryString;
    std::string _pathInfo;
    std::string _completePath;
    std::string _binary;
    
    void extractQueryString(const std::string& uri);
};