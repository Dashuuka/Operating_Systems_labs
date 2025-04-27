#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <stdexcept>
#include <cctype>

struct employee {
    int num;
    char name[10];
    double hours;
};

inline void validateEmployee(const employee& emp) {
    if (emp.num <= 0) {
        throw std::invalid_argument("Employee number must be positive");
    }

    if (emp.hours < 0) {
        throw std::invalid_argument("Negative working hours");
    }

    if (emp.name[0] == '\0') {
        throw std::invalid_argument("Employee name cannot be empty");
    }

    int len = 0;
    bool nullFound = false;
    for (int i = 0; i < 10; i++) {
        if (emp.name[i] == '\0') {
            nullFound = true;
            break;
        }
        len++;
    }
    if (!nullFound) {
        throw std::invalid_argument("Employee name is not null-terminated");
    }

    if (len > 9) {
        throw std::invalid_argument("Employee name is too long");
    }

    for (int i = 0; i < len; i++) {
        if (!std::isprint(static_cast<unsigned char>(emp.name[i]))) {
            throw std::invalid_argument("Employee name contains non-printable characters");
        }
    }

    if (emp.hours > 168.0) {
        throw std::invalid_argument("Working hours exceed maximum allowed limit (168 hours per week)");
    }
}

#endif
