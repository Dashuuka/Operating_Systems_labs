#pragma once

#include <iostream>
#include <string>
#include <cstring>

struct Employee {
    int num;
    char name[10];
    double hours;

    Employee() : num(0), hours(0.0) {
        std::memset(name, 0, sizeof(name));
    }

    Employee(int id, const std::string& empName, double empHours) : num(id), hours(empHours) {
        std::strncpy(name, empName.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }

    void print() const {
        std::cout << "ID: " << num << ", Name: " << name << ", Hours: " << hours << std::endl;
    }
};