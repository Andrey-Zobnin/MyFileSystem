#ifndef MFILE_H
#define MFILE_H

#include <string>
#include "metafile.h"

// Structure to store metadata and file data
struct MFile {
    MetaFile meta; // Metadata
    std::string data; // File data
};

#endif 