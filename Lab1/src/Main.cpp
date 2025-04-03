#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "employee.h"


// Function to print the contents of the binary file to the console.
void printBinaryFile(const std::string& fileName) {
    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error opening binary file: " << fileName << std::endl;
        return;
    }
    employee emp;
    while (ifs.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        std::cout << "Num: " << emp.num << " Name: " << emp.name
            << " Hours: " << emp.hours << std::endl;
    }
}

// Function to print the contents of the report file to the console.
void printReport(const std::string& fileName) {
    std::ifstream ifs(fileName);
    if (!ifs) {
        std::cerr << "Error opening report file: " << fileName << std::endl;
        return;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        std::cout << line << std::endl;
    }
}

int main() {
    std::string binFileName;
    int recordCount = 0;

    std::cout << "Binary file name: ";
    std::cin >> binFileName;
    std::cout << "Number of records: ";
    std::cin >> recordCount;

    // Build the command to execute Creator.exe with required parameters.
    std::string creatorCmd = "Creator.exe " + binFileName + " " + std::to_string(recordCount);
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Launch the Creator process to generate the binary file.
    if (!CreateProcess(NULL, const_cast<char*>(creatorCmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to start Creator process. Error: " << GetLastError() << std::endl;
        return 1;
    }
    // Wait for the Creator process to finish.
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printBinaryFile(binFileName);

    std::string reportFileName;
    double hourlyRate = 0;

    std::cout << "Report file name: ";
    std::cin >> reportFileName;
    std::cout << "Hourly rate: ";
    std::cin >> hourlyRate;

    std::string reporterCmd = "Reporter.exe " + binFileName + " " + reportFileName + " " + std::to_string(hourlyRate);

    if (!CreateProcess(NULL, const_cast<char*>(reporterCmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Failed to start Reporter process. Error: " << GetLastError() << std::endl;
        return 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printReport(reportFileName);
    return 0;
}
