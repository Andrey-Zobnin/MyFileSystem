#include "../include/Directory.h"
#include "../include/File.h"
#include <algorithm>
#include <iostream>

// Constructor for root directory
Directory::Directory(const std::string& dirName) : name(dirName) {
    // Root directory has no parent
}

// Constructor for subdirectory
Directory::Directory(const std::string& dirName, std::shared_ptr<Directory> parentDir) 
    : name(dirName) {
    parent = parentDir;
}

// Get directory name
std::string Directory::getName() const {
    return name;
}

// Set directory name
void Directory::setName(const std::string& dirName) {
    name = dirName;
}

// Get parent directory
std::shared_ptr<Directory> Directory::getParent() const {
    return parent.lock();
}

// Add subdirectory
bool Directory::addSubdirectory(const std::string& dirName) {
    // Check if the directory or file already exists
    if (exists(dirName)) {
        std::cerr << "A directory or file with the name '" << dirName << "' already exists." << std::endl;
        return false;
    }
    
    // Create new directory
    auto newDir = std::make_shared<Directory>(dirName, shared_from_this());
    subdirectories[dirName] = newDir;
    
    return true;
}

// Remove subdirectory
bool Directory::removeSubdirectory(const std::string& dirName) {
    // Check if the directory exists
    if (!isDirectory(dirName)) {
        std::cerr << "Directory '" << dirName << "' does not exist." << std::endl;
        return false;
    }
    
    // Get the directory
    auto dir = subdirectories[dirName];
    
    // Check if directory is empty
    if (!dir->listFiles().empty() || !dir->listSubdirectories().empty()) {
        std::cerr << "Directory '" << dirName << "' is not empty." << std::endl;
        return false;
    }
    
    // Remove the directory
    subdirectories.erase(dirName);
    
    return true;
}

// Get subdirectory
std::shared_ptr<Directory> Directory::getSubdirectory(const std::string& dirName) const {
    auto it = subdirectories.find(dirName);
    if (it != subdirectories.end()) {
        return it->second;
    }
    return nullptr;
}

// List subdirectories
std::vector<std::string> Directory::listSubdirectories() const {
    std::vector<std::string> result;
    for (const auto& pair : subdirectories) {
        result.push_back(pair.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

// Add file
bool Directory::addFile(std::shared_ptr<File> file) {
    // Check if a file or directory with the same name already exists
    if (exists(file->getName())) {
        std::cerr << "A file or directory with the name '" << file->getName() << "' already exists." << std::endl;
        return false;
    }
    
    // Add the file
    files[file->getName()] = file;
    
    return true;
}

// Remove file
bool Directory::removeFile(const std::string& fileName) {
    // Check if the file exists
    if (!isFile(fileName)) {
        std::cerr << "File '" << fileName << "' does not exist." << std::endl;
        return false;
    }
    
    // Remove the file
    files.erase(fileName);
    
    return true;
}

// Get file
std::shared_ptr<File> Directory::getFile(const std::string& fileName) const {
    auto it = files.find(fileName);
    if (it != files.end()) {
        return it->second;
    }
    return nullptr;
}

// List files
std::vector<std::string> Directory::listFiles() const {
    std::vector<std::string> result;
    for (const auto& pair : files) {
        result.push_back(pair.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

// Check if an item with the given name exists
bool Directory::exists(const std::string& name) const {
    return (subdirectories.find(name) != subdirectories.end() || 
            files.find(name) != files.end());
}

// Check if the item is a file
bool Directory::isFile(const std::string& name) const {
    return (files.find(name) != files.end());
}

// Check if the item is a directory
bool Directory::isDirectory(const std::string& name) const {
    return (subdirectories.find(name) != subdirectories.end());
}
