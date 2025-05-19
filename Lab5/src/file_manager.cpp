#include "file_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

FileManager::FileManager(const std::string& filename) : filename(filename) {}

FileManager::~FileManager() {}

bool FileManager::createFile(const std::vector<Employee>& employees) {
    std::lock_guard<std::mutex> lock(fileMutex);

    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return false;
    }

    for (const auto& emp : employees) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }

    file.close();
    return true;
}

void FileManager::printFile() const {
    std::lock_guard<std::mutex> lock(fileMutex);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    Employee emp;
    std::cout << "File contents:" << std::endl;
    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        emp.print();
    }

    file.close();
}

long FileManager::getPositionById(int id) const {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return -1;
    }

    Employee emp;
    long position = 0;

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        if (emp.num == id) {
            file.close();
            return position;
        }
        position += sizeof(Employee);
    }

    file.close();
    return -1;
}

bool FileManager::getEmployee(int id, Employee& employee) {
    std::lock_guard<std::mutex> lock(fileMutex);

    long position = getPositionById(id);
    if (position == -1) {
        return false;
    }

    std::ifstream file(filename, std::ios::binary);
    file.seekg(position);
    file.read(reinterpret_cast<char*>(&employee), sizeof(Employee));
    file.close();

    return true;
}

bool FileManager::updateEmployee(const Employee& employee) {
    std::lock_guard<std::mutex> lock(fileMutex);

    long position = getPositionById(employee.num);
    if (position == -1) {
        return false;
    }

    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(position);
    file.write(reinterpret_cast<const char*>(&employee), sizeof(Employee));
    file.close();

    return true;
}

bool FileManager::acquireReadLock(int employeeId, int clientId) {
    std::lock_guard<std::mutex> lockGuard(locksMutex);

    if (locks.find(employeeId) != locks.end()) {
        bool hasWriteLock = std::any_of(locks[employeeId].begin(), locks[employeeId].end(),
            [](const auto& pair) { return pair.second; });

        if (hasWriteLock) {
            return false;
        }
    }

    locks[employeeId].push_back({ clientId, false });

    if (recordMutexes.find(employeeId) == recordMutexes.end()) {
        recordMutexes[employeeId] = std::make_unique<std::shared_mutex>();
    }

    return true;
}

bool FileManager::acquireWriteLock(int employeeId, int clientId) {
    std::lock_guard<std::mutex> lockGuard(locksMutex);

    auto it = locks.find(employeeId);
    if (it != locks.end() && !it->second.empty()) {
        return false;
    }

    if (locks.find(employeeId) == locks.end()) {
        locks[employeeId] = std::vector<std::pair<int, bool>>();
    }
    locks[employeeId].push_back({ clientId, true });

    if (recordMutexes.find(employeeId) == recordMutexes.end()) {
        recordMutexes[employeeId] = std::make_unique<std::shared_mutex>();
    }

    return true;
}

void FileManager::releaseLock(int employeeId, int clientId) {
    std::lock_guard<std::mutex> lockGuard(locksMutex);

    auto it = locks.find(employeeId);
    if (it != locks.end()) {
        auto& clientLocks = it->second;

        auto removeIt = std::remove_if(clientLocks.begin(), clientLocks.end(),
            [clientId](const auto& pair) { return pair.first == clientId; });

        if (removeIt != clientLocks.end()) {
            clientLocks.erase(removeIt, clientLocks.end());

            if (clientLocks.empty()) {
                locks.erase(it);
            }
        }
    }
}