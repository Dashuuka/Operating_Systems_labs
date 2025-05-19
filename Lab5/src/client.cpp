#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
#include <thread>
#include <chrono>

#include "employee.h"
#include "messages.h"

const std::string SERVER_PIPE = "\\\\.\\pipe\\server_pipe";
const std::string CLIENT_PIPE_PREFIX = "\\\\.\\pipe\\client_pipe_";

void printMenu() {
    std::cout << "\nMenu:" << std::endl;
    std::cout << "1. Modify employee record" << std::endl;
    std::cout << "2. Read employee record" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Choose option: ";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_id>" << std::endl;
        return 1;
    }

    int clientId = std::stoi(argv[1]);
    std::string clientPipeName = CLIENT_PIPE_PREFIX + std::to_string(clientId);

    std::cout << "Client " << clientId << " starting..." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    HANDLE clientPipe = CreateFileA(
        clientPipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (clientPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening client pipe: " << GetLastError() << std::endl;
        return 1;
    }

    HANDLE serverPipe = CreateFileA(
        SERVER_PIPE.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (serverPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening server pipe: " << GetLastError() << std::endl;
        CloseHandle(clientPipe);
        return 1;
    }

    std::cout << "Client " << clientId << " connected to server" << std::endl;

    bool running = true;
    DWORD bytesRead, bytesWritten;

    while (running) {
        printMenu();

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        Message request, response;
        request.clientId = clientId;

        switch (choice) {
        case 1: { // Модификация записи
            std::cout << "Enter employee ID to modify: ";
            int employeeId;
            std::cin >> employeeId;
            std::cin.ignore();

            request.type = OperationType::WRITE_REQUEST;
            request.employee.num = employeeId;

            if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                std::cerr << "Error writing to client pipe" << std::endl;
                running = false;
                break;
            }

            if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                std::cerr << "Error reading from server pipe" << std::endl;
                running = false;
                break;
            }

            if (response.type == OperationType::EMPLOYEE_DATA) {
                std::cout << "Retrieved employee data:" << std::endl;
                response.employee.print();

                std::string newName;
                std::cout << "\nNew name (max 9 chars, current: " << response.employee.name << "): ";
                std::getline(std::cin, newName);

                double newHours;
                std::cout << "New hours (current: " << response.employee.hours << "): ";
                std::string hoursStr;
                std::getline(std::cin, hoursStr);

                Employee modifiedEmployee = response.employee;

                if (!newName.empty()) {
                    std::strncpy(modifiedEmployee.name, newName.c_str(), sizeof(modifiedEmployee.name) - 1);
                    modifiedEmployee.name[sizeof(modifiedEmployee.name) - 1] = '\0';
                }

                if (!hoursStr.empty()) {
                    newHours = std::stod(hoursStr);
                    modifiedEmployee.hours = newHours;
                }

                std::cout << "Press Enter to send modified data..." << std::endl;
                std::cin.get();

                request.type = OperationType::MODIFIED_DATA;
                request.employee = modifiedEmployee;

                if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                    std::cerr << "Error writing to client pipe" << std::endl;
                    running = false;
                    break;
                }

                if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                    std::cerr << "Error reading from server pipe" << std::endl;
                    running = false;
                    break;
                }

                if (response.type == OperationType::ACCESS_GRANTED) {
                    std::cout << "Employee data updated successfully" << std::endl;
                }
                else {
                    std::cout << "Failed to update employee data" << std::endl;
                }

                std::cout << "Press Enter to release lock..." << std::endl;
                std::cin.get();

                request.type = OperationType::RELEASE_LOCK;

                if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                    std::cerr << "Error writing to client pipe" << std::endl;
                    running = false;
                    break;
                }

                if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                    std::cerr << "Error reading from server pipe" << std::endl;
                    running = false;
                    break;
                }
            }
            else if (response.type == OperationType::EMPLOYEE_NOT_FOUND) {
                std::cout << "Employee with ID " << employeeId << " not found" << std::endl;
            }
            else {
                std::cout << "Access denied. Record is locked by another client" << std::endl;
            }
            break;
        }

        case 2: { // Чтение записи
            std::cout << "Enter employee ID to read: ";
            int employeeId;
            std::cin >> employeeId;
            std::cin.ignore();

            request.type = OperationType::READ_REQUEST;
            request.employee.num = employeeId;

            if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                std::cerr << "Error writing to client pipe" << std::endl;
                running = false;
                break;
            }

            if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                std::cerr << "Error reading from server pipe" << std::endl;
                running = false;
                break;
            }

            if (response.type == OperationType::EMPLOYEE_DATA) {
                std::cout << "Employee data:" << std::endl;
                response.employee.print();

                std::cout << "Press Enter to release lock..." << std::endl;
                std::cin.get();

                request.type = OperationType::RELEASE_LOCK;

                if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                    std::cerr << "Error writing to client pipe" << std::endl;
                    running = false;
                    break;
                }

                if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                    std::cerr << "Error reading from server pipe" << std::endl;
                    running = false;
                    break;
                }
            }
            else if (response.type == OperationType::EMPLOYEE_NOT_FOUND) {
                std::cout << "Employee with ID " << employeeId << " not found" << std::endl;
            }
            else {
                std::cout << "Access denied. Record is locked by another client" << std::endl;
            }
            break;
        }

        case 3: { // Выход
            std::cout << "Exiting..." << std::endl;

            request.type = OperationType::CLIENT_EXIT;

            if (!WriteFile(clientPipe, &request, sizeof(Message), &bytesWritten, NULL)) {
                std::cerr << "Error writing to client pipe" << std::endl;
            }

            if (!ReadFile(serverPipe, &response, sizeof(Message), &bytesRead, NULL)) {
                std::cerr << "Error reading from server pipe" << std::endl;
            }

            running = false;
            break;
        }

        default:
            std::cout << "Invalid option. Try again." << std::endl;
            break;
        }
    }

    CloseHandle(clientPipe);
    CloseHandle(serverPipe);

    std::cout << "Client " << clientId << " terminated" << std::endl;

    return 0;
}