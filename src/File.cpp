#include "../include/File.h"

// Constructor
File::File(const std::string& fileName, int fileOffset, int fileSize)
    : name(fileName), offset(fileOffset), size(fileSize) {
}

// Get file name
std::string File::getName() const {
    return name;
}

// Set file name
void File::setName(const std::string& fileName) {
    name = fileName;
}

// Get file offset
int File::getOffset() const {
    return offset;
}

// Set file offset
void File::setOffset(int fileOffset) {
    offset = fileOffset;
}

// Get file size
int File::getSize() const {
    return size;
}

// Set file size
void File::setSize(int fileSize) {
    size = fileSize;
}
