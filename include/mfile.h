#ifndef MFILE_H
#define MFILE_H

#include <string>
#include "metafile.h"

// Structure to store metadata and file data
struct mfile {
    metafile meta;     // File metadata
    std::string data;  // File content
};

#endif // MFILE_H
