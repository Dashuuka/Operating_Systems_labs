#include <iostream>
#include <fstream>
#include <string>
#include "employee.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: Creator <binary_file> <record_count>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    int recordCount = 0;

    try {
        recordCount = std::stoi(argv[2]);
    }
    catch (...) {
        std::cerr << "Invalid record count" << std::endl;
        return 1;
    }

    std::ofstream ofs(fileName, std::ios::binary);
    if (!ofs) {
        std::cerr << "Cannot open file for writing: " << fileName << std::endl;
        return 1;
    }

    for (int i = 0; i < recordCount; ++i) {
        employee emp = {}; 
        std::cout << "Enter employee " << i + 1 << " (num name hours): ";

        if (!(std::cin >> emp.num >> emp.name >> emp.hours)) {
            std::cerr << "Input error" << std::endl;
            return 1;
        }

        ofs.write(reinterpret_cast<char*>(&emp), sizeof(emp));
        if (!ofs) {
            std::cerr << "Write error" << std::endl;
            return 1;
        }
    }

    ofs.close();
    return 0;
}
