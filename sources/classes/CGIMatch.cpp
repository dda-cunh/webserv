#include "../../includes/classes/CGIMatch.hpp"

CGIMatch::CGIMatch()
{}

CGIMatch::CGIMatch(const std::string& uri, const std::string& binary) {
    parse(uri);
	_binary = binary;
}

CGIMatch& CGIMatch::operator=(const CGIMatch& other) {
    if (this != &other) {
        _basePath = other._basePath;
        _scriptName = other._scriptName;
        _scriptExtension = other._scriptExtension;
        _queryString = other._queryString;
        _pathInfo = other._pathInfo;
        _completePath = other._completePath;
        _binary = other._binary;
    }
    return *this;
}

void CGIMatch::parse(const std::string& uri) {
    size_t dotPos = uri.find('.');
    if (dotPos == std::string::npos) {
        _completePath = uri;
        _basePath.clear();
        _scriptName.clear();
        _scriptExtension.clear();
        _pathInfo.clear();
        _queryString.clear();
        return;
    }

    size_t queryPos = uri.find('?');
    std::string pathPart = (queryPos != std::string::npos) ? 
        uri.substr(0, queryPos) : uri;
    
    size_t lastSlashBeforeDot = uri.rfind('/', dotPos);
    size_t nextSlashAfterDot = pathPart.find('/', dotPos);
    
    _completePath = uri;
    _basePath = uri.substr(0, lastSlashBeforeDot);
    
    size_t scriptEndPos = (nextSlashAfterDot != std::string::npos) ? 
        nextSlashAfterDot : 
        ((queryPos != std::string::npos) ? queryPos : uri.length());
    _scriptName = uri.substr(lastSlashBeforeDot + 1, scriptEndPos - lastSlashBeforeDot - 1);
    
    _scriptExtension = uri.substr(dotPos, scriptEndPos - dotPos);
    
    if (nextSlashAfterDot != std::string::npos) {
        _pathInfo = pathPart.substr(nextSlashAfterDot);
    } else {
        _pathInfo.clear();
    }
    
    extractQueryString(uri);
}

void CGIMatch::extractQueryString(const std::string& uri) {
    _queryString.clear();
    
    size_t queryPos = uri.find('?');
    if (queryPos != std::string::npos) {
        _queryString = uri.substr(queryPos + 1);
    }
}

std::string CGIMatch::getScriptName() const {return _scriptName;}

std::string CGIMatch::getScriptExtension() const {return _scriptExtension;}

std::string CGIMatch::getBasePath() const {return _basePath;}

std::string CGIMatch::getPathInfo() const {return _pathInfo;}

std::string CGIMatch::getQueryString() const {return _queryString;}

std::string CGIMatch::getCompletePath() const {return _completePath;}

std::string CGIMatch::getBinary() const { return _binary;}

std::string CGIMatch::findExtension(const std::string& uri) {
    size_t startPos = uri.find('/', 1);
    if (startPos == std::string::npos) {
        startPos = 0;
    }
    while (startPos != std::string::npos) {
        size_t endPos = uri.find('/', startPos + 1);
        std::string segment = uri.substr(startPos + 1, endPos - startPos - 1);
        size_t extPos = segment.find('.');
        if (extPos != std::string::npos) {
            size_t queryPos = segment.find('?', extPos);
            if (queryPos != std::string::npos) {
                return segment.substr(extPos, queryPos - extPos);
            }
            return segment.substr(extPos);
        }
        startPos = endPos;
    }
    return "";
}

std::ostream& operator<<(std::ostream& out, const CGIMatch& cgi) {
    out << "\t_basePath: " << cgi.getBasePath() << std::endl
        << "\t_scriptName: " << cgi.getScriptName() << std::endl
        << "\t_scriptExtension: " << cgi.getScriptExtension() << std::endl
        << "\t_pathInfo: " << cgi.getPathInfo() << std::endl
        << "\t_queryString: " << cgi.getQueryString() << std::endl
        << "\t_completePath: " << cgi.getCompletePath() << std::endl
        << "\t_binary: " << cgi.getBinary() << std::endl;
    
    return out;
}