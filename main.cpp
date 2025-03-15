#include <iostream>
#include <cstring> // Include for strcmp
#include "metafile.h"
#include "mfile.h"
#include "FileSystem.h"

// Constructor that opens the specified file
FileSystem::FileSystem(const std::string& filename) {
    fileStream.open(filename, std::ios::in | std::ios::out | std::ios::app);
    if (!fileStream.is_open()) {
        std::cerr << "Error opening file system file." << std::endl;
        exit(-1);
    }
    currentOffset = fileStream.tellp(); // Set the current position
}

// Destructor that closes the file
FileSystem::~FileSystem() {
    fileStream.close();
}

// Method to write a file to the file system
void FileSystem::writeFile(const std::string& filename) {
    std::ifstream inputFileStream(filename);
    if (!inputFileStream.is_open()) {
        std::cout << "Error reading file: " << filename << std::endl;
        return;
    }

    MFile myFile; // Use the correct structure name
    memcpy(myFile.meta.name, filename.c_str(), filename.size());
    myFile.meta.name[filename.size()] = '\0'; // Null-terminate the name
    myFile.meta.count = 0;
    myFile.meta.dataOffset = currentOffset; // Set data offset
    std::string line;

    while (getline(inputFileStream, line)) {
        myFile.meta.count += line.size();
        myFile.data += line + "\n"; // Add newline for proper formatting
    }

    // Write metadata
    fileStream.seekp(currentOffset, std::ios::beg);
    fileStream.write((char *)(&(myFile.meta)), sizeof(myFile.meta));
    currentOffset += sizeof(myFile.meta); // Update current offset for data

    // Write data
    fileStream.write(myFile.data.data(), myFile.data.size());
    currentOffset += myFile.data.size(); // Update current offset for next write
    inputFileStream.close();
    std::cout << "File written: " << filename << std::endl;
}

// Method to read a file from the file system
void FileSystem::readFile(const std::string& filename) {
    MFile myFile; // Use the correct structure name
    fileStream.seekg(0, std::ios::beg);
    while (fileStream.read((char *)(&(myFile.meta)), sizeof(myFile.meta))) {
        if (strcmp(myFile.meta.name, filename.c_str()) == 0) {
            myFile.data.resize(myFile.meta.count);
            fileStream.seekg(myFile.meta.dataOffset, std::ios::beg); // Seek to data offset
            fileStream.read(myFile.data.data(), myFile.meta.count);
            std::cout << "Contents of " << filename << ":\n" << myFile.data << std::endl;
            return;
        }
    }
    std::cout << "File not found: " << filename << std::endl;
}

// Method to list all files in the file system
void FileSystem::listFiles() {
    MFile myFile; // Use the correct structure name
    fileStream.seekg(0, std::ios::beg);
    std::cout << "Files in the system:\n";
    while (fileStream.read((char *)(&(myFile.meta)), sizeof(myFile.meta))) {
        std::cout << myFile.meta.name << std::endl;
        fileStream.seekg(myFile.meta.dataOffset + myFile.meta.count, std::ios_base::beg); // Seek to next file
    }
}

int main() {
    FileSystem fileSystem("file.txt"); // Create a FileSystem object with the specified file
    std::string command, filename;

    while (true) {
        std::cout << "Enter command (write/read/list/exit): "; // Prompt user for a command
        std::cin >> command;

        if (command == "write") {
            std::cout << "Enter filename to write: "; // Prompt for the filename to write
            std::cin >> filename;
            fileSystem.writeFile(filename); // Call the writeFile method
        } else if (command == "read") {
            std::cout << "Enter filename to read: "; // Prompt for the filename to read
            std::cin >> filename;
            fileSystem.readFile(filename); // Call the readFile method
        } else if (command == "list") {
            fileSystem.listFiles(); // Call the listFiles method to display all files
        } else if (command == "exit") {
            break; // Exit the loop and terminate the program
        } else {
            std::cout << "Unknown command." << std::endl; // Handle unknown commands
        }
    }

    return 0; // Return success
}