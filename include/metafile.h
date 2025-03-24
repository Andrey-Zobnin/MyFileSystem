#ifndef METAFILE_H
#define METAFILE_H

#include <cstring>

// Structure to store file metadata
struct metafile {
    int count;          // File size
    char name[20];      // File name
};

#endif // METAFILE_H
