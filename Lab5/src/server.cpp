#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <windows.h>
#include <cstring>

#include "employee.h"
#include "messages.h"
#include "file_manager.h"

const std::string SERVER_PIPE = "\\\\.\\pipe\\server_pipe";
const std::string CLIENT_PIPE_PREFIX = "\\\\.\\pipe\\client_pipe_";

void handleClient(int clientId, FileManager& fileManager) {
    std::string serverPipeName = SERVER_PIPE;

    HANDLE serverPipe = CreateNamedPipeA(
        serverPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        sizeof(Message),
        sizeof(Message),
        0,
        NULL
    );

    if (serverPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating server pipe: " << GetLastError() << std::endl;
        return;
    }

    std::string clientPipeName = CLIENT_PIPE_PREFIX + std::to_string(clientId);

    if (!ConnectNamedPipe(serverPipe, NULL)) {
        std::cerr << "Error connecting server pipe: " << GetLastError() << std::endl;
        CloseHandle(serverPipe);
        return;
    }

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
        DisconnectNamedPipe(serverPipe);
        CloseHandle(serverPipe);
        return;
    }

    std::cout << "Server: Connected to client " << clientId << std::endl;

    Message request, response;
    bool running = true;
    DWORD bytesRead, bytesWritten;

    while (running) {
        if (!ReadFile(clientPipe, &request, sizeof(Message), &bytesRead, NULL) || bytesRead == 0) {
            std::cerr << "Error reading from client pipe or client disconnected" << std::endl;
            break;
        }

        switch (request.type) {
        case OperationType::READ_REQUEST: {
            std::cout << "Server: Read request from client " << clientId << " for employee ID " << request.employee.num << std::endl;

            if (fileManager.acquireReadLock(request.employee.num, clientId)) {
                Employee emp;
                if (fileManager.getEmployee(request.employee.num, emp)) {
                    response = Message(OperationType::EMPLOYEE_DATA, clientId, emp);
                }
                else {
                    response = Message(OperationType::EMPLOYEE_NOT_FOUND, clientId);
                }
            }
            else {
                response = Message(OperationType::ACCESS_DENIED, clientId);
            }
            break;
        }

        case OperationType::WRITE_REQUEST: {
            std::cout << "Server: Write request from client " << clientId << " for employee ID " << request.employee.num << std::endl;

            if (fileManager.acquireWriteLock(request.employee.num, clientId)) {
                Employee emp;
                if (fileManager.getEmployee(request.employee.num, emp)) {
                    response = Message(OperationType::EMPLOYEE_DATA, clientId, emp);
                }
                else {
                    fileManager.releaseLock(request.employee.num, clientId);
                    response = Message(OperationType::EMPLOYEE_NOT_FOUND, clientId);
                }
            }
            else {
                response = Message(OperationType::ACCESS_DENIED, clientId);
            }
            break;
        }

        case OperationType::MODIFIED_DATA: {
            std::cout << "Server: Received modified data from client " << clientId << " for employee ID " << request.employee.num << std::endl;

            if (fileManager.updateEmployee(request.employee)) {
                response = Message(OperationType::ACCESS_GRANTED, clientId);
            }
            else {
                response = Message(OperationType::ACCESS_DENIED, clientId);
            }
            break;
        }

        case OperationType::RELEASE_LOCK: {
            std::cout << "Server: Release lock request from client " << clientId << " for employee ID " << request.employee.num << std::endl;

            fileManager.releaseLock(request.employee.num, clientId);
            response = Message(OperationType::ACCESS_GRANTED, clientId);
            break;
        }

        case OperationType::CLIENT_EXIT: {
            std::cout << "Server: Client " << clientId << " is exiting" << std::endl;
            response = Message(OperationType::ACCESS_GRANTED, clientId);
            running = false;
            break;
        }

        default:
            response = Message(OperationType::ACCESS_DENIED, clientId);
            break;
        }

        if (!WriteFile(serverPipe, &response, sizeof(Message), &bytesWritten, NULL)) {
            std::cerr << "Error writing to server pipe" << std::endl;
            break;
        }
    }

    DisconnectNamedPipe(serverPipe);
    CloseHandle(serverPipe);
    CloseHandle(clientPipe);

    std::cout << "Server: Disconnected from client " << clientId << std::endl;
}

void launchClient(int clientId) {
    std::string clientPipeName = CLIENT_PIPE_PREFIX + std::to_string(clientId);

    HANDLE clientPipe = CreateNamedPipeA(
        clientPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        sizeof(Message),
        sizeof(Message),
        0,
        NULL
    );

    if (clientPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Error creating client pipe: " << GetLastError() << std::endl;
        return;
    }

    std::string commandLine = "client.exe " + std::to_string(clientId);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(
        NULL,
        (LPSTR)commandLine.c_str(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        std::cerr << "Error launching client process: " << GetLastError() << std::endl;
        CloseHandle(clientPipe);
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(clientPipe);
}

int main() {
    std::cout << "Server starting..." << std::endl;

    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;

    FileManager fileManager(filename);

    int numEmployees;
    std::cout << "Enter number of employees: ";
    std::cin >> numEmployees;
    std::cin.ignore();

    std::vector<Employee> employees;

    for (int i = 0; i < numEmployees; ++i) {
        int id;
        std::string name;
        double hours;

        std::cout << "Employee #" << i + 1 << std::endl;
        std::cout << "ID: ";
        std::cin >> id;
        std::cin.ignore();

        std::cout << "Name (max 9 chars): ";
        std::getline(std::cin, name);

        std::cout << "Hours: ";
        std::cin >> hours;
        std::cin.ignore();

        employees.push_back(Employee(id, name, hours));
    }

    if (!fileManager.createFile(employees)) {
        std::cerr << "Error creating file" << std::endl;
        return 1;
    }

    std::cout << "\nInitial file content:" << std::endl;
    fileManager.printFile();

    int numClients;
    std::cout << "\nEnter number of clients to launch: ";
    std::cin >> numClients;

    std::vector<std::thread> clientThreads;

    for (int i = 1; i <= numClients; ++i) {
        launchClient(i);
        clientThreads.push_back(std::thread(handleClient, i, std::ref(fileManager)));
    }

    std::cout << "Server is running. Press Enter to exit after all clients have finished." << std::endl;
    std::cin.ignore();
    std::cin.get();

    for (auto& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "\nFinal file content:" << std::endl;
    fileManager.printFile();

    std::cout << "Server shutdown complete" << std::endl;

    return 0;
}