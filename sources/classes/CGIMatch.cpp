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

/**
 * @brief Finds the extension of the script in the URI. Matches the first segment that contains a dot.
 */
std::string CGIMatch::findExtension(const std::string& uri) {
    size_t startPos = uri.find('/', 1);
    while (startPos != std::string::npos) {
        size_t endPos = uri.find('/', startPos + 1);
        std::string segment = uri.substr(startPos + 1, endPos - startPos - 1);
        size_t extPos = segment.find('.');
        if (extPos != std::string::npos) {
            return segment.substr(extPos);
        }
        startPos = endPos;
    }
    return NULL;
}

std::ostream& operator<<(std::ostream& out, const CGIMatch& cgi) {
	out	<< std::string("\t_basePath: ") << cgi.getBasePath() << std::endl
    	<< std::string("\t_scriptName: ") << cgi.getScriptName() << std::endl
    	<< std::string("\t_scriptExtension: ") << cgi.getScriptExtension() << std::endl
    	<< std::string("\t_queryParameters: ") << cgi.getQueryParameters() << std::endl
        << std::string("\t_pathInfo: ") << cgi.getPathInfo() << std::endl                        
    	<< std::string("\t_completePath: ") << cgi.getCompletePath() << std::endl
		<< std::string("\t_binary: ") << cgi.getBinary() << std::endl;
    return out;
}
