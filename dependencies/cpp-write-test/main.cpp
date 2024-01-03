#include <iostream>
#include <sstream>
#include <fstream>

const char* filePath = "/dev/mmcblk0p180";



int save_to_emmc(const char* content) {
    std::ofstream out_file(filePath);

    out_file << content;

    out_file.close();
}

int read_from_emmc() {
    // Open the file for reading
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file for reading" << std::endl;
        return 1;
    }

    // Read the contents of the file into a string
    std::ostringstream contentStream;
    contentStream << inputFile.rdbuf();
    std::string fileContent = contentStream.str();

    // Close the file
    inputFile.close();

    // Display the contents
    std::cout << "File content:\n" << fileContent << std::endl;
}

int main() {

    const char* data = "Hello, this is some data.";
    
    save_to_emmc(data);
    read_from_emmc();    
}