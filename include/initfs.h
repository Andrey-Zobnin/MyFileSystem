#ifndef INITFS_H
#define INITFS_H

#include <cstring>

// Structure to store file system metadata
struct initfs {
    int offset;          // Offset to the file table
    int count;           // Number of files
    int fileEndOffset;   // End offset for files
    int dirTableOffset;  // Offset to the directory table
    int dirCount;        // Number of directories
};

#endif // INITFS_H
