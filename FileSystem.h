#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <fstream>
#include <string>
#include "mfile.h"

// Class representing a simple file system
class FileSystem {
private:
    std::fstream f; // File stream for working with the file system

public:
    // Constructor that opens the specified file
    FileSystem(const std::string& filename);
    
    // Destructor that closes the file
    ~FileSystem();

    // Method to write a file to the file system
    void writeFile(const std::string& filename);

    // Method to read a file from the file system
    void readFile(const std::string& filename);

    // Method to list all files in the file system
    void listFiles();
};

#endif 