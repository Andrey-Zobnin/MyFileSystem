#ifndef METAFILE_H
#define METAFILE_H

#include <cstring>

// Structure to store file metadata
struct MetaFile {
    int count; // Number of bytes in data
    char name[25]; // File name
    long dataOffset; // Offset of data in the file
};

#endif 