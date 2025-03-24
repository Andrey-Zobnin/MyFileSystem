#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Utility functions
namespace Utils {
    // Split a string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join a vector of strings with a delimiter
    std::string join(const std::vector<std::string>& elements, const std::string& delimiter);
    
    // Check if a path is absolute
    bool isAbsolutePath(const std::string& path);
    
    // Normalize a path (remove redundant separators, resolve .. and .)
    std::string normalizePath(const std::string& path);
    
    // Get parent directory path
    std::string getParentPath(const std::string& path);
    
    // Get filename from path
    std::string getFilename(const std::string& path);
    
    // Combine paths
    std::string combinePaths(const std::string& path1, const std::string& path2);
}

#endif // UTILS_H
