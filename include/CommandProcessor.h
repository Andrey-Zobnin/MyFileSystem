#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <string>
#include <vector>
#include "FileSystem.h"

// Class for processing user commands
class CommandProcessor {
private:
    FileSystem& fs;

    // Parse a command line into command and arguments
    std::vector<std::string> parseCommand(const std::string& commandLine);
    
    // Command handlers
    void handleHelp();
    void handleWrite(const std::vector<std::string>& args);
    void handleRead(const std::vector<std::string>& args);
    void handleList(const std::vector<std::string>& args);
    void handleChangeDir(const std::vector<std::string>& args);
    void handleMakeDir(const std::vector<std::string>& args);
    void handleRemoveDir(const std::vector<std::string>& args);
    void handleRemoveFile(const std::vector<std::string>& args);
    void handlePwd();
    void handleInit(const std::vector<std::string>& args);

public:
    // Constructor
    CommandProcessor(FileSystem& fileSystem);
    
    // Process a command
    bool processCommand(const std::string& commandLine);
    
    // Display prompt
    void displayPrompt() const;
};

#endif // COMMAND_PROCESSOR_H
