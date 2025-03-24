#include <iostream>
#include <string>
#include <memory>
#include "../include/FileSystem.h"

void displayHelp() {
    std::cout << "\nFile System Commands:\n";
    std::cout << "===========================\n";
    std::cout << "ls, dir             - List files and directories\n";
    std::cout << "cd <path>           - Change directory\n";
    std::cout << "mkdir <path>        - Create directory\n";
    std::cout << "rmdir <path>        - Remove directory\n";
    std::cout << "write <path> <src>  - Write file to filesystem\n";
    std::cout << "read <path> <dst>   - Read file from filesystem\n";
    std::cout << "delete <path>       - Delete file\n";
    std::cout << "copy <src> <dst>    - Copy file\n";
    std::cout << "move <src> <dst>    - Move file\n";
    std::cout << "pwd                 - Print current directory\n";
    std::cout << "help                - Show this help\n";
    std::cout << "exit, quit          - Exit program\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Virtual File System\n";
    std::cout << "==================\n";
    
    // Get file system name from command line or use default
    std::string fsName = "vfs.dat";
    if (argc > 1) {
        fsName = argv[1];
    }
    
    // Initialize file system
    try {
        // Use direct unique_ptr construction instead of make_unique for C++11 compatibility
        std::unique_ptr<FileSystem> fs(new FileSystem(fsName));
        std::cout << "Opened file system: " << fsName << std::endl;
        
        // Display help initially
        displayHelp();
        
        // Main command loop
        std::string command;
        bool running = true;
        
        while (running) {
            std::cout << fs->getCurrentPath() << "> ";
            std::getline(std::cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            // Check for exit command
            if (command == "exit" || command == "quit") {
                running = false;
            } else {
                // Process other commands
                fs->processCommand(command);
            }
        }
        
        std::cout << "Filesystem closed. Goodbye!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
