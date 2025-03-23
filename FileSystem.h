#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <fstream>
#include <string>
#include <vector>
#include "mfile.h"

// Class representing a simple file system
class FileSystem {
private:
    std::fstream fileStream; // File stream for working with the file system
    long currentOffset; // Current position for writing data
    std::string currentDirectory; // Current directory path

public:
    // Constructor that opens the specified file
    FileSystem(const std::string& filename);
    
    // Destructor that closes the file
    ~FileSystem();

    // Method to write a file to the file system
    void writeFile(const std::string& filename);

    // Method to read a file from the file system
    void readFile(const std::string& filename);

    // Method to list all files in the current directory
    void listFiles();

    // Method to change the current directory
    void changeDirectory(const std::string& directory);
};

#endif 