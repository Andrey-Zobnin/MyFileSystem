#include <iostream>
#include <fstream>
#include "FileSystem.h"
#include "test.h" // Include the test header

void testWriteFile() {
    FileSystem fs("test_file.txt");
    std::ofstream testFile("input.txt");
    testFile << "Hello, World!" << std::endl;
    testFile.close();

    fs.writeFile("input.txt");

    // Check if the file was written correctly
    std::ifstream f("test_file.txt");
    std::string line;
    bool found = false;
    while (getline(f, line)) {
        if (line.find("Hello, World!") != std::string::npos) {
            found = true;
            break;
        }
    }
    f.close();

    std::cout << "Test Write File: " << (found ? "Passed" : "Failed") << std::endl;
}

void testReadFile() {
    FileSystem fs("test_file.txt");
    fs.readFile("input.txt"); // This should read the content we wrote in the previous test
}

void testListFiles() {
    FileSystem fs("test_file.txt");
    fs.listFiles(); // This should list the files in the file system
}

int main() {
    testWriteFile();
    testReadFile();
    testListFiles();
    return 0;
}