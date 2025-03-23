#ifndef METAFILE_H
#define METAFILE_H

#include <cstring>

// Structure to store file metadata
struct initfs {
    int offset;
    int count;
    //    int fileOffset;
    int fileEndOffset;
};
#endif 