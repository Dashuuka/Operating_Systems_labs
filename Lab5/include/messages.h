#pragma once

#include "employee.h"

enum class OperationType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    EMPLOYEE_DATA = 3,
    ACCESS_GRANTED = 4,
    ACCESS_DENIED = 5,
    RELEASE_LOCK = 6,
    MODIFIED_DATA = 7,
    EMPLOYEE_NOT_FOUND = 8,
    CLIENT_EXIT = 9
};

struct Message {
    OperationType type;
    int clientId;
    Employee employee;

    Message() : type(OperationType::EMPLOYEE_NOT_FOUND), clientId(0) {}

    Message(OperationType msgType, int client, const Employee& emp = Employee())
        : type(msgType), clientId(client), employee(emp) {}
};