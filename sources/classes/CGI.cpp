#include "../../includes/classes/CGI.hpp"


CGI::CGI(const std::string& uri, const std::string& binary) {
    parse(uri);
	_binary = binary;
}

void CGI::parse(const std::string& uri) {
    size_t pos = uri.find_last_of('.');
    if (pos != std::string::npos) {
        size_t lastSlashPos = uri.rfind('/');
        if (lastSlashPos != std::string::npos && lastSlashPos < pos) {
            _scriptName = uri.substr(lastSlashPos + 1, pos - lastSlashPos - 1);
        } else {
            _scriptName.clear();
        }
        
        _scriptExtension = uri.substr(pos);
        _completePath = uri;
        _basePath = uri.substr(0, lastSlashPos);
        extractQueryParameters(uri);
    }
}

void CGI::extractQueryParameters(const std::string& uri) {

    size_t questionMarkPos = uri.find('?');
    if (questionMarkPos != std::string::npos) {
        _queryParameters = uri.substr(questionMarkPos + 1);
    } else {
        _queryParameters.clear();
    }
}

std::string CGI::getScriptName() const {
    return _scriptName;
}

std::string CGI::getScriptExtension() const {
    return _scriptExtension;
}

std::string CGI::getCompletePath() const {
    return _completePath;
}

std::string CGI::getBasePath() const {
    return _basePath;
}

std::string CGI::getQueryParameters() const {
    return _queryParameters;
}

std::string CGI::getBinary() const {
	return _binary;
}

std::ostream& operator<<(std::ostream& out, const CGI& cgi) {
	out	<< std::string("_basePath: ") << cgi.getBasePath() << std::endl
    	<< std::string("_scriptName: ") << cgi.getScriptName() << std::endl
    	<< std::string("_scriptExtension: ") << cgi.getScriptExtension() << std::endl
    	<< std::string("_queryParameters: ") << cgi.getBasePath() << std::endl
    	<< std::string("_completePath: ") << cgi.getCompletePath() << std::endl
		<< std::string("_binary: ") << cgi.getBinary() << std::endl;
    return out;
}
