#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


void writeErrorToFile(const std::string& filename, const std::string& error_message) {
    std::ofstream error_file(filename);
    if (error_file.is_open()) {
        error_file << error_message << std::endl;
        error_file.close();
    } else {
        std::cerr << "Unable to open error file: " << filename << std::endl;
    }
}



