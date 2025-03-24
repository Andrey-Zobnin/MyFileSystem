#ifndef FILE_H
#define FILE_H

#include <string>
#include <memory>

// Class representing a file in the file system
class File {
private:
    std::string name;     // File name
    int offset;           // Offset in the filesystem file
    int size;             // Size of the file
    
public:
    // Constructor
    File(const std::string& fileName, int fileOffset = -1, int fileSize = 0);
    
    // Getters and setters
    std::string getName() const;
    void setName(const std::string& fileName);
    int getOffset() const;
    void setOffset(int fileOffset);
    int getSize() const;
    void setSize(int fileSize);
};

#endif // FILE_H
