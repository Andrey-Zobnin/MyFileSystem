#include <iostream>
#include <memory>
#include "../include/FileSystem.h"
#include "../include/web/WebServer.h"

int main(int argc, char* argv[]) {
    std::string fsFile = "vfs.dat";
    
    // If a file name is provided as a command line argument, use it
    if (argc > 1) {
        fsFile = argv[1];
    }
    
    std::cout << "Virtual File System Web Interface" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // Create or open the file system
    auto fileSystem = std::make_shared<FileSystem>(fsFile);
    std::cout << "Opened file system: " << fsFile << std::endl;
    
    // Create and start the web server
    WebServer server(fileSystem);
    
    std::cout << "Starting web server on port 5000..." << std::endl;
    std::cout << "Navigate to http://localhost:5000 in your browser" << std::endl;
    
    server.start();
    
    return 0;
}