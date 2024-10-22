#include "../../includes/classes/CGIMatch.hpp"


CGIMatch::CGIMatch(const std::string& uri, const std::string& binary) {
    parse(uri);
	_binary = binary;
}

void CGIMatch::parse(const std::string& uri) {
    size_t dotPos = uri.find('.');
    if (dotPos == std::string::npos) {
        _completePath = uri;
        _basePath.clear();
        _scriptName.clear();
        _scriptExtension.clear();
        _pathInfo.clear();
        _queryParameters.clear();
        return;
    }

    size_t lastSlashBeforeDot = uri.rfind('/', dotPos);
    size_t nextSlashAfterDot = uri.find('/', dotPos);

    _completePath = uri;
    _basePath = uri.substr(0, lastSlashBeforeDot);
    _scriptName = uri.substr(lastSlashBeforeDot + 1, nextSlashAfterDot - lastSlashBeforeDot - 1);
    _scriptExtension = uri.substr(dotPos, nextSlashAfterDot - dotPos);

    if (nextSlashAfterDot != std::string::npos) {
        _pathInfo = uri.substr(nextSlashAfterDot);
    } else {
        _pathInfo.clear();
    }

    _queryParameters.clear();
}

void CGIMatch::extractQueryParameters(const std::string& uri) {

    size_t questionMarkPos = uri.find('?');
    if (questionMarkPos != std::string::npos) {
        _queryParameters = uri.substr(questionMarkPos + 1);
    } else {
        _queryParameters.clear();
    }
}

std::string CGIMatch::getScriptName() const {
    return _scriptName;
}

std::string CGIMatch::getScriptExtension() const {
    return _scriptExtension;
}

std::string CGIMatch::getBasePath() const {
    return _basePath;
}

std::string CGIMatch::getQueryParameters() const {
    return _queryParameters;
}

std::string CGIMatch::getPathInfo() const {
    return _pathInfo;
}

std::string CGIMatch::getCompletePath() const {
    return _completePath;
}

std::string CGIMatch::getBinary() const {
	return _binary;
}

std::ostream& operator<<(std::ostream& out, const CGIMatch& cgi) {
	out	<< std::string("_basePath: ") << cgi.getBasePath() << std::endl
    	<< std::string("_scriptName: ") << cgi.getScriptName() << std::endl
    	<< std::string("_scriptExtension: ") << cgi.getScriptExtension() << std::endl
    	<< std::string("_queryParameters: ") << cgi.getQueryParameters() << std::endl
        << std::string("_pathInfo: ") << cgi.getPathInfo() << std::endl                        
    	<< std::string("_completePath: ") << cgi.getCompletePath() << std::endl
		<< std::string("_binary: ") << cgi.getBinary() << std::endl;
    return out;
}
