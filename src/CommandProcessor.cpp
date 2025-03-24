#include "../include/CommandProcessor.h"
#include <iostream>
#include <sstream>
#include <algorithm>

CommandProcessor::CommandProcessor(FileSystem& fileSystem) : fs(fileSystem) {}

std::vector<std::string> CommandProcessor::parseCommand(const std::string& commandLine) {
    std::vector<std::string> result;
    std::istringstream iss(commandLine);
    std::string token;
    
    while (iss >> token) {
        result.push_back(token);
    }
    
    return result;
}

bool CommandProcessor::processCommand(const std::string& commandLine) {
    std::vector<std::string> tokens = parseCommand(commandLine);
    
    if (tokens.empty()) {
        return true;
    }
    
    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    if (command == "exit" || command == "quit") {
        return false;
    } else if (command == "help") {
        handleHelp();
    } else if (command == "write") {
        handleWrite(args);
    } else if (command == "read") {
        handleRead(args);
    } else if (command == "ls" || command == "list") {
        handleList(args);
    } else if (command == "cd") {
        handleChangeDir(args);
    } else if (command == "mkdir") {
        handleMakeDir(args);
    } else if (command == "rmdir") {
        handleRemoveDir(args);
    } else if (command == "rm") {
        handleRemoveFile(args);
    } else if (command == "pwd") {
        handlePwd();
    } else if (command == "init") {
        handleInit(args);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for a list of available commands." << std::endl;
    }
    
    return true;
}

void CommandProcessor::displayPrompt() const {
    std::cout << fs.getCurrentPath() << "> ";
}

void CommandProcessor::handleHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help                - Show this help message" << std::endl;
    std::cout << "  write <file> <text> - Write text to a file" << std::endl;
    std::cout << "  read <file>         - Read a file" << std::endl;
    std::cout << "  ls [path]           - List files and directories" << std::endl;
    std::cout << "  cd <path>           - Change directory" << std::endl;
    std::cout << "  pwd                 - Print current directory" << std::endl;
    std::cout << "  mkdir <path>        - Create a directory" << std::endl;
    std::cout << "  rmdir <path>        - Remove a directory" << std::endl;
    std::cout << "  rm <file>           - Remove a file" << std::endl;
    std::cout << "  init <file>         - Initialize a new file system" << std::endl;
    std::cout << "  exit, quit          - Exit the program" << std::endl;
}

void CommandProcessor::handleWrite(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: write <file> <text>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    std::string content;
    
    for (size_t i = 1; i < args.size(); ++i) {
        content += args[i];
        if (i < args.size() - 1) {
            content += " ";
        }
    }
    
    if (fs.writeFile(path, content)) {
        std::cout << "File written successfully." << std::endl;
    } else {
        std::cout << "Failed to write file." << std::endl;
    }
}

void CommandProcessor::handleRead(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: read <file>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    std::string content;
    
    if (fs.readFile(path, content)) {
        std::cout << "File content:" << std::endl;
        std::cout << content << std::endl;
    } else {
        std::cout << "Failed to read file." << std::endl;
    }
}

void CommandProcessor::handleList(const std::vector<std::string>& args) {
    // Change directory if path is provided, then list current directory contents
    if (!args.empty()) {
        std::string path = args[0];
        if (!fs.changeDirectory(path)) {
            std::cout << "Failed to change to directory: " << path << std::endl;
            return;
        }
    }
    
    // List contents of current directory
    fs.listFiles();
    fs.listDirectories();
}

void CommandProcessor::handleChangeDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: cd <path>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    
    if (fs.changeDirectory(path)) {
        std::cout << "Current directory: " << fs.getCurrentPath() << std::endl;
    } else {
        std::cout << "Failed to change directory." << std::endl;
    }
}

void CommandProcessor::handleMakeDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: mkdir <path>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    
    if (fs.createDirectory(path)) {
        std::cout << "Directory created successfully." << std::endl;
    } else {
        std::cout << "Failed to create directory." << std::endl;
    }
}

void CommandProcessor::handleRemoveDir(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: rmdir <path>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    
    if (fs.removeDirectory(path)) {
        std::cout << "Directory removed successfully." << std::endl;
    } else {
        std::cout << "Failed to remove directory." << std::endl;
    }
}

void CommandProcessor::handleRemoveFile(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: rm <file>" << std::endl;
        return;
    }
    
    std::string path = args[0];
    
    if (fs.deleteFile(path)) {
        std::cout << "File removed successfully." << std::endl;
    } else {
        std::cout << "Failed to remove file." << std::endl;
    }
}

void CommandProcessor::handlePwd() {
    std::cout << "Current directory: " << fs.getCurrentPath() << std::endl;
}

void CommandProcessor::handleInit(const std::vector<std::string>& args) {
    std::string filename = "mydump";
    
    if (!args.empty()) {
        filename = args[0];
    }
    
    // Creating a new FileSystem object will initialize it
    FileSystem newFs(filename);
    std::cout << "File system initialized: " << filename << std::endl;
}
