#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include "employee.h"

struct EmployeeReport {
    employee emp;
    double salary;
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: Reporter <binary_file> <report_file> <hourly_rate>" << std::endl;
        return 1;
    }

    std::string binFileName = argv[1];
    std::string reportFileName = argv[2];
    double hourlyRate = 0;

    try {
        hourlyRate = std::stod(argv[3]);
    }
    catch (...) {
        std::cerr << "Invalid hourly rate" << std::endl;
        return 1;
    }

    std::ifstream ifs(binFileName, std::ios::binary);
    if (!ifs) {
        std::cerr << "Cannot open binary file: " << binFileName << std::endl;
        return 1;
    }

    std::vector<EmployeeReport> reports;
    employee emp;
    while (ifs.read(reinterpret_cast<char*>(&emp), sizeof(emp))) {
        EmployeeReport er;
        er.emp = emp;
        er.salary = emp.hours * hourlyRate; 
        reports.push_back(er);
    }
    ifs.close(); 

    std::sort(reports.begin(), reports.end(), [](const EmployeeReport& a, const EmployeeReport& b) {
        return a.emp.num < b.emp.num;
        });

    std::ofstream ofs(reportFileName);
    if (!ofs) {
        std::cerr << "Cannot open report file: " << reportFileName << std::endl;
        return 1;
    }

    ofs << "Report for file \"" << binFileName << "\"\n";
    ofs << "Num\tName\tHours\tSalary\n";

    for (const auto& er : reports) {
        ofs << er.emp.num << "\t" << er.emp.name << "\t"
            << er.emp.hours << "\t" << std::fixed << std::setprecision(2)
            << er.salary << "\n";
    }

    ofs.close();
    return 0;
}
