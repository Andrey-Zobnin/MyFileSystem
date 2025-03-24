#include "../include/FileSystem.h"
#include "../include/File.h"
#include "../include/mfile.h"
#include "../include/pair.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

// Constructor - initializes or opens an existing file system
FileSystem::FileSystem(const std::string& filename) : fsFileName(filename), initialized(false), isRunning(true) {
    // Open the file system file
    fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    // If file doesn't exist, create it and initialize
    if (!fileStream.is_open()) {
        fileStream.clear();
        fileStream.open(filename, std::ios::out | std::ios::binary);
        fileStream.close();
        fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        initializeFileSystem();
    } else {
        // Load existing file system
        loadDirectoryStructure();
    }
    
    // Start worker threads for async operations
    for (int i = 0; i < 2; ++i) { // Create 2 worker threads
        threadPool.emplace_back(&FileSystem::workerThread, this);
    }
    
    initialized = true;
}

// Destructor - ensures proper cleanup
FileSystem::~FileSystem() {
    // Signal threads to stop and wait for them
    {
        std::unique_lock<std::mutex> lock(fileSystemMutex);
        isRunning = false;
    }
    queueCondition.notify_all();
    
    for (auto& thread : threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Save any pending changes
    saveDirectoryStructure();
    
    // Close the file system file
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

// Initialize a new file system
void FileSystem::initializeFileSystem() {
    fsInfo.offset = 100;        // Start of file table
    fsInfo.count = 0;           // No files initially
    fsInfo.fileEndOffset = 1000; // Start of file data
    fsInfo.dirTableOffset = 500; // Start of directory table
    fsInfo.dirCount = 1;        // Just the root directory initially
    
    // Write file system metadata
    fileStream.seekp(0, std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fsInfo), sizeof(fsInfo));
    
    // Create root directory
    root = std::make_shared<Directory>("/");
    currentDir = root;
    currentPath = "/";
    
    // Save the initial directory structure
    saveDirectoryStructure();
}

// Load directory structure from the file system file
void FileSystem::loadDirectoryStructure() {
    // Read file system metadata
    fileStream.seekg(0, std::ios::beg);
    fileStream.read(reinterpret_cast<char*>(&fsInfo), sizeof(fsInfo));
    
    // Create root directory
    root = std::make_shared<Directory>("/");
    currentDir = root;
    currentPath = "/";
    
    // TODO: Implement reading directory structure from file
    // For now, just create empty root directory
}

// Save directory structure to the file system file
void FileSystem::saveDirectoryStructure() {
    // Write file system metadata
    fileStream.seekp(0, std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fsInfo), sizeof(fsInfo));
    
    // TODO: Implement saving directory structure to file
    // For now, just update the metadata
}

// Normalize path (handle '.', '..', multiple slashes, etc.)
std::string FileSystem::normalizePath(const std::string& path) {
    if (path.empty()) return currentPath;
    
    std::string normalizedPath;
    std::vector<std::string> components;
    std::string currentComponent;
    
    // Determine if path is absolute or relative
    bool isAbsolute = (path[0] == '/');
    
    // Start with current path for relative paths
    if (!isAbsolute) {
        components = splitPath(currentPath);
    }
    
    // Split the path and process each component
    std::istringstream pathStream(path);
    std::string component;
    
    while (std::getline(pathStream, component, '/')) {
        if (component.empty() || component == ".") {
            // Skip empty components and "."
            continue;
        } else if (component == "..") {
            // Go up one level
            if (!components.empty()) {
                components.pop_back();
            }
        } else {
            // Add valid component
            components.push_back(component);
        }
    }
    
    // Construct normalized path
    if (isAbsolute) {
        normalizedPath = "/";
    }
    
    for (size_t i = 0; i < components.size(); ++i) {
        if (i > 0 || !isAbsolute) {
            normalizedPath += "/";
        }
        normalizedPath += components[i];
    }
    
    // Handle empty normalized path (means root)
    if (normalizedPath.empty()) {
        normalizedPath = "/";
    }
    
    return normalizedPath;
}

// Split path into components
std::vector<std::string> FileSystem::splitPath(const std::string& path) {
    std::vector<std::string> components;
    std::istringstream pathStream(path);
    std::string component;
    
    while (std::getline(pathStream, component, '/')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

// Find directory from path
std::shared_ptr<Directory> FileSystem::findDirectory(const std::string& path) {
    std::string normalizedPath = normalizePath(path);
    std::vector<std::string> components = splitPath(normalizedPath);
    
    std::shared_ptr<Directory> current = root;
    
    // Navigate through components
    for (const auto& component : components) {
        current = current->getSubdirectory(component);
        if (!current) {
            return nullptr; // Directory not found
        }
    }
    
    return current;
}

// Find file from path
std::shared_ptr<File> FileSystem::findFile(const std::string& path) {
    // Extract directory path and filename
    size_t lastSlash = path.find_last_of('/');
    std::string dirPath = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : "/";
    std::string fileName = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    
    // If the path doesn't start with /, it's relative to current directory
    if (dirPath.empty() || dirPath[0] != '/') {
        dirPath = currentPath + "/" + dirPath;
    }
    
    // Normalize the directory path
    dirPath = normalizePath(dirPath);
    
    // Find the directory
    std::shared_ptr<Directory> dir = findDirectory(dirPath);
    if (!dir) {
        return nullptr; // Directory not found
    }
    
    // Find the file in the directory
    return dir->getFile(fileName);
}

// Write file to the file system
bool FileSystem::writeFile(const std::string& path, const std::string& sourceFilename) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Extract directory path and filename
    size_t lastSlash = path.find_last_of('/');
    std::string dirPath = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : "/";
    std::string fileName = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    
    // If the path doesn't contain a directory, use current directory
    if (dirPath.empty() || (lastSlash == std::string::npos && path[0] != '/')) {
        dirPath = currentPath;
    }
    
    // Find the directory
    std::shared_ptr<Directory> dir = findDirectory(dirPath);
    if (!dir) {
        std::cerr << "Directory not found: " << dirPath << std::endl;
        return false;
    }
    
    // Check if file already exists
    if (dir->isFile(fileName)) {
        std::cerr << "File already exists: " << fileName << std::endl;
        return false;
    }
    
    // Read the source file
    std::ifstream sourceFile(sourceFilename, std::ios::binary);
    if (!sourceFile) {
        std::cerr << "Cannot open source file: " << sourceFilename << std::endl;
        return false;
    }
    
    // Read file content
    std::string fileContent;
    char buffer[4096];
    while (sourceFile.read(buffer, sizeof(buffer))) {
        fileContent.append(buffer, sizeof(buffer));
    }
    fileContent.append(buffer, sourceFile.gcount());
    sourceFile.close();
    
    // Create metafile structure
    mfile newFile;
    strncpy(newFile.meta.name, fileName.c_str(), sizeof(newFile.meta.name) - 1);
    newFile.meta.name[sizeof(newFile.meta.name) - 1] = '\0';
    newFile.meta.count = fileContent.size();
    newFile.data = fileContent;
    
    // Write file to the file system
    int fileOffset = fsInfo.fileEndOffset;
    
    // Set file position
    fileStream.seekp(fileOffset, std::ios::beg);
    
    // Write file metadata and data
    fileStream.write(reinterpret_cast<char*>(&newFile.meta), sizeof(newFile.meta));
    fileStream.write(newFile.data.c_str(), newFile.data.size());
    
    // Create file entry
    pair fileEntry;
    strncpy(fileEntry.name, fileName.c_str(), sizeof(fileEntry.name) - 1);
    fileEntry.name[sizeof(fileEntry.name) - 1] = '\0';
    fileEntry.offset = fileOffset;
    
    // Write file entry to file table
    fileStream.seekp(fsInfo.offset + fsInfo.count * sizeof(pair), std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fileEntry), sizeof(fileEntry));
    
    // Update file system metadata
    fsInfo.count++;
    fsInfo.fileEndOffset += sizeof(newFile.meta) + newFile.data.size();
    
    // Update metadata in file
    fileStream.seekp(0, std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fsInfo), sizeof(fsInfo));
    
    // Create file object
    auto file = std::make_shared<File>(fileName, fileOffset, newFile.data.size());
    
    // Add file to directory
    dir->addFile(file);
    
    return true;
}

// Read file from the file system
bool FileSystem::readFile(const std::string& path, const std::string& destFilename) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Find the file
    std::shared_ptr<File> file = findFile(path);
    if (!file) {
        std::cerr << "File not found: " << path << std::endl;
        return false;
    }
    
    // Seek to the file's position
    fileStream.seekg(file->getOffset(), std::ios::beg);
    
    // Read file metadata
    metafile meta;
    fileStream.read(reinterpret_cast<char*>(&meta), sizeof(meta));
    
    // Read file data
    std::string data;
    data.resize(meta.count);
    fileStream.read(&data[0], meta.count);
    
    // Write data to destination file
    std::ofstream destFile(destFilename, std::ios::binary);
    if (!destFile) {
        std::cerr << "Cannot create destination file: " << destFilename << std::endl;
        return false;
    }
    
    destFile.write(data.c_str(), data.size());
    destFile.close();
    
    return true;
}

// Delete file from the file system
bool FileSystem::deleteFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Extract directory path and filename
    size_t lastSlash = path.find_last_of('/');
    std::string dirPath = (lastSlash != std::string::npos) ? path.substr(0, lastSlash) : "/";
    std::string fileName = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1) : path;
    
    // If the path doesn't contain a directory, use current directory
    if (dirPath.empty() || (lastSlash == std::string::npos && path[0] != '/')) {
        dirPath = currentPath;
    }
    
    // Find the directory
    std::shared_ptr<Directory> dir = findDirectory(dirPath);
    if (!dir) {
        std::cerr << "Directory not found: " << dirPath << std::endl;
        return false;
    }
    
    // Check if file exists
    if (!dir->isFile(fileName)) {
        std::cerr << "File not found: " << fileName << std::endl;
        return false;
    }
    
    // Remove file from directory
    return dir->removeFile(fileName);
    
    // Note: We're not actually removing the file data from the file system file
    // In a real implementation, we might want to:
    // 1. Mark the file as deleted in the file table
    // 2. Implement a garbage collection or defragmentation mechanism
}

// Copy file within the file system
bool FileSystem::copyFile(const std::string& sourcePath, const std::string& destPath) {
    // Find source file
    std::shared_ptr<File> sourceFile = findFile(sourcePath);
    if (!sourceFile) {
        std::cerr << "Source file not found: " << sourcePath << std::endl;
        return false;
    }
    
    // Extract destination directory path and filename
    size_t lastSlash = destPath.find_last_of('/');
    std::string dirPath = (lastSlash != std::string::npos) ? destPath.substr(0, lastSlash) : "/";
    std::string fileName = (lastSlash != std::string::npos) ? destPath.substr(lastSlash + 1) : destPath;
    
    // If the path doesn't contain a directory, use current directory
    if (dirPath.empty() || (lastSlash == std::string::npos && destPath[0] != '/')) {
        dirPath = currentPath;
    }
    
    // Find the destination directory
    std::shared_ptr<Directory> dir = findDirectory(dirPath);
    if (!dir) {
        std::cerr << "Destination directory not found: " << dirPath << std::endl;
        return false;
    }
    
    // Check if file already exists at destination
    if (dir->isFile(fileName)) {
        std::cerr << "File already exists at destination: " << fileName << std::endl;
        return false;
    }
    
    // Read source file content
    fileStream.seekg(sourceFile->getOffset(), std::ios::beg);
    
    // Read file metadata
    metafile meta;
    fileStream.read(reinterpret_cast<char*>(&meta), sizeof(meta));
    
    // Read file data
    std::string data;
    data.resize(meta.count);
    fileStream.read(&data[0], meta.count);
    
    // Create new file entry
    int fileOffset = fsInfo.fileEndOffset;
    
    // Set file position
    fileStream.seekp(fileOffset, std::ios::beg);
    
    // Update metadata with new filename
    strcpy(meta.name, fileName.c_str());
    
    // Write file metadata and data
    fileStream.write(reinterpret_cast<char*>(&meta), sizeof(meta));
    fileStream.write(data.c_str(), data.size());
    
    // Create file entry
    pair fileEntry;
    strncpy(fileEntry.name, fileName.c_str(), sizeof(fileEntry.name) - 1);
    fileEntry.name[sizeof(fileEntry.name) - 1] = '\0';
    fileEntry.offset = fileOffset;
    
    // Write file entry to file table
    fileStream.seekp(fsInfo.offset + fsInfo.count * sizeof(pair), std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fileEntry), sizeof(fileEntry));
    
    // Update file system metadata
    fsInfo.count++;
    fsInfo.fileEndOffset += sizeof(meta) + data.size();
    
    // Update metadata in file
    fileStream.seekp(0, std::ios::beg);
    fileStream.write(reinterpret_cast<char*>(&fsInfo), sizeof(fsInfo));
    
    // Create file object
    auto newFile = std::make_shared<File>(fileName, fileOffset, data.size());
    
    // Add file to directory
    dir->addFile(newFile);
    
    return true;
}

// Move file within the file system
bool FileSystem::moveFile(const std::string& sourcePath, const std::string& destPath) {
    // Copy the file
    if (!copyFile(sourcePath, destPath)) {
        return false;
    }
    
    // Delete the original
    return deleteFile(sourcePath);
}

// Create directory
bool FileSystem::createDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Normalize the path
    std::string normalizedPath = normalizePath(path);
    
    // Check if directory already exists
    if (findDirectory(normalizedPath)) {
        std::cerr << "Directory already exists: " << normalizedPath << std::endl;
        return false;
    }
    
    // Extract parent directory path and new directory name
    size_t lastSlash = normalizedPath.find_last_of('/');
    std::string parentPath = (lastSlash != std::string::npos) ? normalizedPath.substr(0, lastSlash) : "/";
    std::string dirName = (lastSlash != std::string::npos) ? normalizedPath.substr(lastSlash + 1) : normalizedPath;
    
    if (parentPath.empty()) {
        parentPath = "/";
    }
    
    // Find parent directory
    std::shared_ptr<Directory> parentDir = findDirectory(parentPath);
    if (!parentDir) {
        std::cerr << "Parent directory not found: " << parentPath << std::endl;
        return false;
    }
    
    // Create the new directory
    return parentDir->addSubdirectory(dirName);
}

// Remove directory
bool FileSystem::removeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Normalize the path
    std::string normalizedPath = normalizePath(path);
    
    // Don't allow removing root
    if (normalizedPath == "/") {
        std::cerr << "Cannot remove root directory" << std::endl;
        return false;
    }
    
    // Extract parent directory path and directory name
    size_t lastSlash = normalizedPath.find_last_of('/');
    std::string parentPath = (lastSlash != std::string::npos) ? normalizedPath.substr(0, lastSlash) : "/";
    std::string dirName = (lastSlash != std::string::npos) ? normalizedPath.substr(lastSlash + 1) : normalizedPath;
    
    if (parentPath.empty()) {
        parentPath = "/";
    }
    
    // Find parent directory
    std::shared_ptr<Directory> parentDir = findDirectory(parentPath);
    if (!parentDir) {
        std::cerr << "Parent directory not found: " << parentPath << std::endl;
        return false;
    }
    
    // Remove the directory
    return parentDir->removeSubdirectory(dirName);
}

// Change current directory
bool FileSystem::changeDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    // Normalize the path
    std::string normalizedPath = normalizePath(path);
    
    // Find the directory
    std::shared_ptr<Directory> dir = findDirectory(normalizedPath);
    if (!dir) {
        std::cerr << "Directory not found: " << normalizedPath << std::endl;
        return false;
    }
    
    // Change current directory
    currentDir = dir;
    currentPath = normalizedPath;
    
    return true;
}

// Get current path
std::string FileSystem::getCurrentPath() const {
    return currentPath;
}

// List files in current directory
void FileSystem::listFiles() {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    std::vector<std::string> files = currentDir->listFiles();
    
    if (files.empty()) {
        std::cout << "No files in current directory" << std::endl;
        return;
    }
    
    std::cout << "Files in " << currentPath << ":" << std::endl;
    for (const auto& file : files) {
        std::cout << "  " << file << std::endl;
    }
}

// List subdirectories in current directory
void FileSystem::listDirectories() {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    std::vector<std::string> directories = currentDir->listSubdirectories();
    
    if (directories.empty()) {
        std::cout << "No subdirectories in current directory" << std::endl;
        return;
    }
    
    std::cout << "Directories in " << currentPath << ":" << std::endl;
    for (const auto& dir : directories) {
        std::cout << "  " << dir << "/" << std::endl;
    }
}

// List all items in current directory
void FileSystem::listAll() {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    std::vector<std::string> directories = currentDir->listSubdirectories();
    std::vector<std::string> files = currentDir->listFiles();
    
    if (directories.empty() && files.empty()) {
        std::cout << "Directory is empty" << std::endl;
        return;
    }
    
    std::cout << "Contents of " << currentPath << ":" << std::endl;
    
    for (const auto& dir : directories) {
        std::cout << "  " << dir << "/" << std::endl;
    }
    
    for (const auto& file : files) {
        std::cout << "  " << file << std::endl;
    }
}

// Async file writing
void FileSystem::writeFileAsync(const std::string& path, const std::string& sourceFilename) {
    std::unique_lock<std::mutex> lock(fileSystemMutex);
    
    // Add task to queue
    taskQueue.push([this, path, sourceFilename]() {
        this->writeFile(path, sourceFilename);
    });
    
    lock.unlock();
    queueCondition.notify_one();
}

// Async file reading
void FileSystem::readFileAsync(const std::string& path, const std::string& destFilename) {
    std::unique_lock<std::mutex> lock(fileSystemMutex);
    
    // Add task to queue
    taskQueue.push([this, path, destFilename]() {
        this->readFile(path, destFilename);
    });
    
    lock.unlock();
    queueCondition.notify_one();
}

// Worker thread function for async operations
void FileSystem::workerThread() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(fileSystemMutex);
            queueCondition.wait(lock, [this]() {
                return !taskQueue.empty() || !isRunning;
            });
            
            if (!isRunning && taskQueue.empty()) {
                return;
            }
            
            task = taskQueue.front();
            taskQueue.pop();
        }
        
        // Execute the task
        task();
    }
}

// Process command from user interface
void FileSystem::processCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Convert command to lowercase
    std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (cmd == "ls" || cmd == "dir") {
        // List contents of current directory
        listAll();
    } else if (cmd == "cd") {
        // Change directory
        std::string path;
        iss >> path;
        if (changeDirectory(path)) {
            std::cout << "Current directory: " << getCurrentPath() << std::endl;
        }
    } else if (cmd == "mkdir") {
        // Create directory
        std::string path;
        iss >> path;
        if (createDirectory(path)) {
            std::cout << "Directory created: " << path << std::endl;
        }
    } else if (cmd == "rmdir") {
        // Remove directory
        std::string path;
        iss >> path;
        if (removeDirectory(path)) {
            std::cout << "Directory removed: " << path << std::endl;
        }
    } else if (cmd == "write" || cmd == "put") {
        // Write file
        std::string path, sourceFile;
        iss >> path >> sourceFile;
        if (writeFile(path, sourceFile)) {
            std::cout << "File written: " << path << std::endl;
        }
    } else if (cmd == "read" || cmd == "get") {
        // Read file
        std::string path, destFile;
        iss >> path >> destFile;
        if (readFile(path, destFile)) {
            std::cout << "File read: " << path << " -> " << destFile << std::endl;
        }
    } else if (cmd == "delete" || cmd == "rm") {
        // Delete file
        std::string path;
        iss >> path;
        if (deleteFile(path)) {
            std::cout << "File deleted: " << path << std::endl;
        }
    } else if (cmd == "copy" || cmd == "cp") {
        // Copy file
        std::string source, dest;
        iss >> source >> dest;
        if (copyFile(source, dest)) {
            std::cout << "File copied: " << source << " -> " << dest << std::endl;
        }
    } else if (cmd == "move" || cmd == "mv") {
        // Move file
        std::string source, dest;
        iss >> source >> dest;
        if (moveFile(source, dest)) {
            std::cout << "File moved: " << source << " -> " << dest << std::endl;
        }
    } else if (cmd == "pwd") {
        // Print working directory
        std::cout << "Current directory: " << getCurrentPath() << std::endl;
    } else if (cmd == "help") {
        // Show help
        std::cout << "Available commands:" << std::endl;
        std::cout << "  ls, dir             - List files and directories" << std::endl;
        std::cout << "  cd <path>           - Change directory" << std::endl;
        std::cout << "  mkdir <path>        - Create directory" << std::endl;
        std::cout << "  rmdir <path>        - Remove directory" << std::endl;
        std::cout << "  write, put <path> <sourcefile> - Write file to filesystem" << std::endl;
        std::cout << "  read, get <path> <destfile>    - Read file from filesystem" << std::endl;
        std::cout << "  delete, rm <path>   - Delete file" << std::endl;
        std::cout << "  copy, cp <src> <dst> - Copy file" << std::endl;
        std::cout << "  move, mv <src> <dst> - Move file" << std::endl;
        std::cout << "  pwd                 - Print current directory" << std::endl;
        std::cout << "  exit, quit          - Exit program" << std::endl;
    } else if (cmd == "exit" || cmd == "quit") {
        // Exit is handled by the main loop
        std::cout << "Exiting..." << std::endl;
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
    }
}

// Returns a list of items in the directory with their types
// Each pair contains: <name, isDirectory>
std::vector<std::pair<std::string, bool>> FileSystem::listDirectory(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileSystemMutex);
    
    std::vector<std::pair<std::string, bool>> items;
    
    // Find directory at the specified path
    std::shared_ptr<Directory> dir;
    if (path == "/") {
        dir = root;
    } else {
        dir = findDirectory(path);
    }
    
    if (!dir) {
        throw std::runtime_error("Directory not found: " + path);
    }
    
    // List subdirectories
    std::vector<std::string> directories = dir->listSubdirectories();
    for (const auto& dirName : directories) {
        items.push_back(std::make_pair(dirName, true)); // true indicates directory
    }
    
    // List files
    std::vector<std::string> files = dir->listFiles();
    for (const auto& fileName : files) {
        items.push_back(std::make_pair(fileName, false)); // false indicates file
    }
    
    return items;
}
