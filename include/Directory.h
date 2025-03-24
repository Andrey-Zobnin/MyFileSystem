#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// Forward declarations to avoid circular dependencies
class File;

// Class representing a directory in the file system
class Directory : public std::enable_shared_from_this<Directory> {
private:
    std::string name;                                              // Directory name
    std::weak_ptr<Directory> parent;                               // Parent directory
    std::unordered_map<std::string, std::shared_ptr<Directory>> subdirectories; // Subdirectories
    std::unordered_map<std::string, std::shared_ptr<File>> files;  // Files in this directory
    
public:
    // Constructors
    Directory(const std::string& dirName);
    Directory(const std::string& dirName, std::shared_ptr<Directory> parentDir);
    
    // Getters and setters
    std::string getName() const;
    void setName(const std::string& dirName);
    std::shared_ptr<Directory> getParent() const;
    
    // Directory operations
    bool addSubdirectory(const std::string& dirName);
    bool removeSubdirectory(const std::string& dirName);
    std::shared_ptr<Directory> getSubdirectory(const std::string& dirName) const;
    std::vector<std::string> listSubdirectories() const;
    
    // File operations
    bool addFile(std::shared_ptr<File> file);
    bool removeFile(const std::string& fileName);
    std::shared_ptr<File> getFile(const std::string& fileName) const;
    std::vector<std::string> listFiles() const;
    
    // Utility functions
    bool exists(const std::string& name) const;
    bool isFile(const std::string& name) const;
    bool isDirectory(const std::string& name) const;
};

#endif // DIRECTORY_H
