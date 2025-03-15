#ifndef METAFILE_H
#define METAFILE_H

#include <cstring>

// Structure to store file metadata
struct metafile {
    int count; // Number of bytes in data
    char name[20]; // File name
    long data_offset; // Offset of data in the file
};

#endif 