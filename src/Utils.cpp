#include "../include/Utils.h"
#include <sstream>
#include <algorithm>

namespace Utils {

    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }
    
    std::string join(const std::vector<std::string>& elements, const std::string& delimiter) {
        std::string result;
        
        for (size_t i = 0; i < elements.size(); ++i) {
            result += elements[i];
            if (i < elements.size() - 1) {
                result += delimiter;
            }
        }
        
        return result;
    }
    
    bool isAbsolutePath(const std::string& path) {
        return !path.empty() && path[0] == '/';
    }
    
    std::string normalizePath(const std::string& path) {
        if (path.empty()) {
            return "/";
        }
        
        // Split path by '/'
        std::vector<std::string> parts = split(path, '/');
        std::vector<std::string> normalized;
        
        for (const auto& part : parts) {
            if (part == "..") {
                if (!normalized.empty()) {
                    normalized.pop_back();
                }
            } else if (part != "." && !part.empty()) {
                normalized.push_back(part);
            }
        }
        
        if (normalized.empty()) {
            return "/";
        }
        
        std::string result;
        if (isAbsolutePath(path)) {
            result = "/";
        }
        
        result += join(normalized, "/");
        
        return result;
    }
    
    std::string getParentPath(const std::string& path) {
        if (path.empty() || path == "/") {
            return "/";
        }
        
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == 0 || lastSlash == std::string::npos) {
            return "/";
        }
        
        return path.substr(0, lastSlash);
    }
    
    std::string getFilename(const std::string& path) {
        if (path.empty() || path == "/") {
            return "";
        }
        
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos) {
            return path;
        }
        
        if (lastSlash == path.size() - 1) {
            return "";
        }
        
        return path.substr(lastSlash + 1);
    }
    
    std::string combinePaths(const std::string& path1, const std::string& path2) {
        if (path1.empty()) {
            return path2;
        }
        
        if (path2.empty()) {
            return path1;
        }
        
        if (isAbsolutePath(path2)) {
            return path2;
        }
        
        std::string result = path1;
        if (result.back() != '/') {
            result += '/';
        }
        
        if (path2.front() == '/') {
            result += path2.substr(1);
        } else {
            result += path2;
        }
        
        return normalizePath(result);
    }
}
