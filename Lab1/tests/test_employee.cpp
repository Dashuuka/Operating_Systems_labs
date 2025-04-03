#include <gtest/gtest.h>
#include "employee.h"
#include <cstring>
#include <stdexcept>

// Test salary calculation for a valid employee.
TEST(EmployeeTest, SalaryCalculation) {
    employee emp;
    emp.num = 1;
    std::strcpy(emp.name, "Alice");
    emp.hours = 40.0;
    EXPECT_NO_THROW(validateEmployee(emp));
    double hourlyRate = 20.0;
    double expected = 800.0;
    double result = emp.hours * hourlyRate;
    EXPECT_DOUBLE_EQ(expected, result);
}

// Test that a valid employee passes all validations.
TEST(EmployeeTest, ValidEmployeePassesValidation) {
    employee emp;
    emp.num = 5;
    std::strcpy(emp.name, "Valid");
    emp.hours = 37.5;
    EXPECT_NO_THROW(validateEmployee(emp));
}

// Test: negative working hours should throw.
TEST(EmployeeTest, NegativeHoursThrows) {
    employee emp;
    emp.num = 2;
    std::strcpy(emp.name, "Bob");
    emp.hours = -1.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: working hours exceeding 168 should throw.
TEST(EmployeeTest, ExceedMaxHoursThrows) {
    employee emp;
    emp.num = 3;
    std::strcpy(emp.name, "Carol");
    emp.hours = 200.0; // > 168
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: employee number must be positive.
TEST(EmployeeTest, InvalidEmployeeNumberThrows) {
    employee emp;
    emp.num = 0;
    std::strcpy(emp.name, "Dave");
    emp.hours = 40.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: empty employee name should throw.
TEST(EmployeeTest, EmptyNameThrows) {
    employee emp;
    emp.num = 4;
    emp.name[0] = '\0';
    emp.hours = 40.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: name not null-terminated within the buffer should throw.
TEST(EmployeeTest, NameNotNullTerminatedThrows) {
    employee emp;
    emp.num = 5;
    for (int i = 0; i < 10; i++) {
        emp.name[i] = 'A';
    }
    emp.hours = 40.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: name that is too long (i.e. no space for null terminator) should throw.
TEST(EmployeeTest, NameTooLongThrows) {
    employee emp;
    emp.num = 6;
    for (int i = 0; i < 10; i++) {
        emp.name[i] = 'B';
    }
    emp.hours = 40.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

// Test: name containing non-printable character should throw.
TEST(EmployeeTest, NameWithNonPrintableCharacterThrows) {
    employee emp;
    emp.num = 7;
    std::strcpy(emp.name, "John");
    emp.name[2] = '\n';
    emp.hours = 40.0;
    EXPECT_THROW(validateEmployee(emp), std::invalid_argument);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
