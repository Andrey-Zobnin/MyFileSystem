#include <iostream> // Include for input/output operations
#include "metafile.h" // Include the metafile structure
#include "mfile.h" // Include the mfile structure
#include "FileSystem.h" // Include the FileSystem class

// Constructor that opens the specified file
FileSystem::FileSystem(const std::string& filename) {
    f.open(filename, std::ios::in | std::ios::out | std::ios::app);
    if (!f.is_open()) {
        std::cerr << "Error opening file system file." << std::endl;
        exit(-1);
    }
}

// Destructor that closes the file
FileSystem::~FileSystem() {
    f.close();
}

// Method to write a file to the file system
void FileSystem::writeFile(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cout << "Error reading file: " << filename << std::endl;
        return;
    }

    mfile my_file;
    memcpy(my_file.meta.name, filename.c_str(), filename.size());
    my_file.meta.name[filename.size()] = '\0'; // Null-terminate the name
    my_file.meta.count = 0;
    std::string line;

    while (getline(ifs, line)) {
        my_file.meta.count += line.size();
        my_file.data += line + "\n"; // Add newline for proper formatting
    }

    f.seekg(0, std::ios::end);
    f.write((char *)(&(my_file.meta)), sizeof(my_file.meta));
    f.write(my_file.data.data(), my_file.data.size());
    ifs.close();
    std::cout << "File written: " << filename << std::endl;
}

// Method to read a file from the file system
void FileSystem::readFile(const std::string& filename) {
    mfile my_file;
    f.seekg(0, std::ios::beg);
    while (f.read((char *)(&(my_file.meta)), sizeof(my_file.meta))) {
        if (strcmp(my_file.meta.name, filename.c_str()) == 0) {
            my_file.data.resize(my_file.meta.count);
            f.read(my_file.data.data(), my_file.meta.count);
            std::cout << "Contents of " << filename << ":\n" << my_file.data << std::endl;
            return;
        }
        f.seekg(my_file.meta.count, std::ios_base::cur);
    }
    std::cout << "File not found: " << filename << std::endl;
}

// Method to list all files in the file system
void FileSystem::listFiles() {
    mfile my_file;
    f.seekg(0, std::ios::beg);
    std::cout << "Files in the system:\n";
    while (f.read((char *)(&(my_file.meta)), sizeof(my_file.meta))) {
        std::cout << my_file.meta.name << std::endl;
        f.seekg(my_file.meta.count, std::ios_base::cur);
    }
}

int main() {
    FileSystem fs("file.txt"); // Create a FileSystem object with the specified file
    std::string command, filename;

    while (true) {
        std::cout << "Enter command (write/read/list/exit): "; // Prompt user for a command
        std::cin >> command;

        if (command == "write") {
            std::cout << "Enter filename to write: "; // Prompt for the filename to write
            std::cin >> filename;
            fs.writeFile(filename); // Call the writeFile method
        } else if (command == "read") {
            std::cout << "Enter filename to read: "; // Prompt for the filename to read
            std::cin >> filename;
            fs.readFile(filename); // Call the readFile method
        } else if (command == "list") {
            fs.listFiles(); // Call the listFiles method to display all files
        } else if (command == "exit") {
            break; // Exit the loop and terminate the program
        } else {
            std::cout << "Unknown command." << std::endl; // Handle unknown commands
        }
    }

    return 0; // Return success
}