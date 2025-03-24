#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <cstring>
#include <vector>

// Structure to store file system initialization data
struct InitFS {
    int offset;           // Offset where the file table begins
    int count;            // Number of files/directories in the file system
    int fileEndOffset;    // Offset where the next file should be written
};

// Structure to store file metadata
struct MetaFile {
    int count;            // Size of the file in bytes
    char name[20];        // Name of the file
    bool isDirectory;     // Flag to identify if it's a directory
    char parentDir[256];  // Path to parent directory
};

// Structure to store file table entry
struct FileTableEntry {
    char name[20];        // Name of the file or directory
    int offset;           // Offset where the file data begins
    bool isDirectory;     // Flag to identify if it's a directory
    char parentDir[256];  // Path to parent directory
};

// Structure to store file and its data
struct MFile {
    MetaFile meta;
    std::string data;
};

#endif // METADATA_H
