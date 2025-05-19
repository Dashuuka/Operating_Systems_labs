#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include "employee.h"

class FileManager {
public:
    FileManager(const std::string& filename);
    ~FileManager();

    bool createFile(const std::vector<Employee>& employees);
    void printFile() const;
    bool getEmployee(int id, Employee& employee);
    bool updateEmployee(const Employee& employee);
    bool acquireReadLock(int employeeId, int clientId);
    bool acquireWriteLock(int employeeId, int clientId);
    void releaseLock(int employeeId, int clientId);

private:
    std::string filename;
    mutable std::mutex fileMutex;
    std::unordered_map<int, std::unique_ptr<std::shared_mutex>> recordMutexes;
    std::unordered_map<int, std::vector<std::pair<int, bool>>> locks;
    std::mutex locksMutex;
    long getPositionById(int id) const;
};