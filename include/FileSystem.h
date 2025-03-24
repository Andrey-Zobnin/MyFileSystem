#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <fstream>          // Provides functionality for file input and output (reading from and writing to files).

#include <string>          // Includes the standard string class for handling text strings.

#include <vector>          // Provides the vector container, a dynamic array that can grow in size.

#include <unordered_map>   // Implements a hash table-based map for fast key-value pair storage and retrieval.

#include <memory>          // Includes smart pointers for automatic memory management (e.g., std::shared_ptr, std::unique_ptr).

#include <mutex>           // Provides mutual exclusion primitives for thread synchronization.

#include <condition_variable> // Allows threads to wait for certain conditions to be met, facilitating thread communication.

#include <thread>          // Provides functionality for creating and managing threads.

#include <queue>           // Implements the queue container, a first-in-first-out (FIFO) data structure.

#include <functional>      // Includes function objects, function pointers, and utilities for working with functions.

#include <utility>         // Provides utility functions and classes, including std::pair for storing two related values.
#include "Directory.h"
#include "File.h"
#include "initfs.h"

// Class representing an object-oriented file system with directory navigation
class FileSystem {
private:
    std::fstream fileStream;          // File stream for working with the file system
    initfs fsInfo;                    // File system metadata
    std::string fsFileName;           // Name of the file system file
    std::shared_ptr<Directory> root;  // Root directory
    std::shared_ptr<Directory> currentDir; // Current directory
    std::string currentPath;          // Current path for display
    bool initialized;                 // Flag indicating if file system is initialized
    
    // Threading components for async operations
    std::mutex fileSystemMutex;       // Mutex for thread safety
    std::condition_variable queueCondition; // For thread synchronization
    std::queue<std::function<void()>> taskQueue; // Task queue for async operations
    bool isRunning;                   // Flag for thread pool operation
    std::vector<std::thread> threadPool; // Thread pool
    
    // Private methods
    void initializeFileSystem();
    void loadDirectoryStructure();
    void saveDirectoryStructure();
    std::string normalizePath(const std::string& path);
    std::shared_ptr<Directory> findDirectory(const std::string& path);
    std::shared_ptr<File> findFile(const std::string& path);
    std::vector<std::string> splitPath(const std::string& path);
    
    // Async operation handler
    void workerThread();
    
public:
    // Constructor that opens or creates the specified file system file
    FileSystem(const std::string& filename);
    
    // Destructor that ensures everything is saved and closed
    ~FileSystem();
    
    // Basic file operations
    bool writeFile(const std::string& path, const std::string& sourceFilename);
    bool readFile(const std::string& path, const std::string& destFilename);
    bool deleteFile(const std::string& path);
    bool copyFile(const std::string& sourcePath, const std::string& destPath);
    bool moveFile(const std::string& sourcePath, const std::string& destPath);
    
    // Directory operations
    bool createDirectory(const std::string& path);
    bool removeDirectory(const std::string& path);
    bool changeDirectory(const std::string& path);
    std::string getCurrentPath() const;
    
    // Listing operations
    void listFiles();
    void listDirectories();
    void listAll();
    
    // Returns a list of items in the directory with their types
    // Each pair contains: <name, isDirectory>
    std::vector<std::pair<std::string, bool>> listDirectory(const std::string& path);
    
    // Async operations
    void writeFileAsync(const std::string& path, const std::string& sourceFilename);
    void readFileAsync(const std::string& path, const std::string& destFilename);
    
    // Command interface
    void processCommand(const std::string& command);
};

#endif