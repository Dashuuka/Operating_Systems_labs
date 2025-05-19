#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <random>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "file_manager.h"

class FileManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        testEmployees = {
            Employee(1, "Alice", 40.0),
            Employee(2, "Bob", 35.5),
            Employee(3, "Charlie", 42.0),
            Employee(4, "David", 38.0),
            Employee(5, "Emma", 44.5)
        };

        fileManager = new FileManager("test_employees.dat");
        fileManager->createFile(testEmployees);
    }

    void TearDown() override {
        delete fileManager;
        remove("test_employees.dat");
    }

    bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

    size_t getFileSize(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        return file.tellg();
    }

    std::vector<Employee> readEmployeesFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        std::vector<Employee> employees;

        Employee emp;
        while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
            employees.push_back(emp);
        }

        return employees;
    }

    FileManager* fileManager;
    std::vector<Employee> testEmployees;
};

TEST_F(FileManagerTest, CreateFile) {
    ASSERT_TRUE(fileExists("test_employees.dat"));
    EXPECT_EQ(getFileSize("test_employees.dat"), testEmployees.size() * sizeof(Employee));

    std::vector<Employee> fileEmployees = readEmployeesFromFile("test_employees.dat");
    ASSERT_EQ(fileEmployees.size(), testEmployees.size());

    for (size_t i = 0; i < testEmployees.size(); ++i) {
        EXPECT_EQ(fileEmployees[i].num, testEmployees[i].num);
        EXPECT_STREQ(fileEmployees[i].name, testEmployees[i].name);
        EXPECT_DOUBLE_EQ(fileEmployees[i].hours, testEmployees[i].hours);
    }
}

TEST_F(FileManagerTest, CreateEmptyFile) {
    FileManager emptyManager("empty_employees.dat");
    std::vector<Employee> emptyEmployees;
    ASSERT_TRUE(emptyManager.createFile(emptyEmployees));

    ASSERT_TRUE(fileExists("empty_employees.dat"));
    EXPECT_EQ(getFileSize("empty_employees.dat"), 0);

    remove("empty_employees.dat");
}

TEST_F(FileManagerTest, CreateFileWithMaxRecords) {
    std::vector<Employee> manyEmployees;
    for (int i = 1; i <= 100; ++i) {
        manyEmployees.push_back(Employee(i, "Employee" + std::to_string(i), 40.0 + i / 10.0));
    }

    FileManager largeManager("large_employees.dat");
    ASSERT_TRUE(largeManager.createFile(manyEmployees));

    ASSERT_TRUE(fileExists("large_employees.dat"));
    EXPECT_EQ(getFileSize("large_employees.dat"), manyEmployees.size() * sizeof(Employee));

    remove("large_employees.dat");
}

TEST_F(FileManagerTest, GetEmployeeById) {
    Employee emp;

    ASSERT_TRUE(fileManager->getEmployee(1, emp));
    EXPECT_EQ(emp.num, 1);
    EXPECT_STREQ(emp.name, "Alice");
    EXPECT_DOUBLE_EQ(emp.hours, 40.0);

    ASSERT_TRUE(fileManager->getEmployee(3, emp));
    EXPECT_EQ(emp.num, 3);
    EXPECT_STREQ(emp.name, "Charlie");
    EXPECT_DOUBLE_EQ(emp.hours, 42.0);

    ASSERT_FALSE(fileManager->getEmployee(999, emp));
    ASSERT_FALSE(fileManager->getEmployee(0, emp));
    ASSERT_FALSE(fileManager->getEmployee(-1, emp));

    ASSERT_TRUE(fileManager->getEmployee(1, emp));
    EXPECT_EQ(emp.num, 1);

    ASSERT_TRUE(fileManager->getEmployee(5, emp));
    EXPECT_EQ(emp.num, 5);
    EXPECT_STREQ(emp.name, "Emma");
}

TEST_F(FileManagerTest, UpdateEmployee) {
    Employee emp(2, "Bobby", 38.0);
    ASSERT_TRUE(fileManager->updateEmployee(emp));

    Employee updatedEmp;
    ASSERT_TRUE(fileManager->getEmployee(2, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 2);
    EXPECT_STREQ(updatedEmp.name, "Bobby");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 38.0);

    Employee emp3(3, "Charles", 43.5);
    ASSERT_TRUE(fileManager->updateEmployee(emp3));

    ASSERT_TRUE(fileManager->getEmployee(3, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 3);
    EXPECT_STREQ(updatedEmp.name, "Charles");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 43.5);

    Employee nonExistentEmp(999, "Nobody", 0.0);
    ASSERT_FALSE(fileManager->updateEmployee(nonExistentEmp));

    ASSERT_TRUE(fileManager->getEmployee(1, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 1);
    EXPECT_STREQ(updatedEmp.name, "Alice");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 40.0);

    Employee emp4(4, "", 39.0);
    ASSERT_TRUE(fileManager->updateEmployee(emp4));

    ASSERT_TRUE(fileManager->getEmployee(4, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 4);
    EXPECT_STREQ(updatedEmp.name, "");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 39.0);

    Employee emp5 = testEmployees[4];
    emp5.hours = 46.0;
    ASSERT_TRUE(fileManager->updateEmployee(emp5));

    ASSERT_TRUE(fileManager->getEmployee(5, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 5);
    EXPECT_STREQ(updatedEmp.name, "Emma");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 46.0);
}

TEST_F(FileManagerTest, ReadLocks) {
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    fileManager->releaseLock(1, 102);
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 103);

    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 102);

    ASSERT_TRUE(fileManager->acquireReadLock(2, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(2, 102));
    fileManager->releaseLock(2, 101);
    fileManager->releaseLock(2, 102);

    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(2, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(3, 103));
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(2, 102);
    fileManager->releaseLock(3, 103);
}

TEST_F(FileManagerTest, WriteLocks) {
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 102));
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));
    ASSERT_TRUE(fileManager->acquireWriteLock(2, 102));

    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(2, 102);

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));
    fileManager->releaseLock(1, 102);

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));
    fileManager->releaseLock(1, 101);

    ASSERT_TRUE(fileManager->acquireWriteLock(999, 101));
    ASSERT_FALSE(fileManager->acquireWriteLock(999, 102));
    fileManager->releaseLock(999, 101);
}

TEST_F(FileManagerTest, ReadWriteContention) {
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 104));

    fileManager->releaseLock(1, 101);
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    fileManager->releaseLock(1, 103);
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    fileManager->releaseLock(1, 104);
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));

    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 103));

    fileManager->releaseLock(1, 102);

    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 103);
}

TEST_F(FileManagerTest, ConcurrentReaders) {
    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    auto readerFunc = [this, &successCount](int clientId) {
        if (fileManager->acquireReadLock(1, clientId)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            Employee emp;
            if (fileManager->getEmployee(1, emp) && emp.num == 1) {
                successCount++;
            }

            fileManager->releaseLock(1, clientId);
        }
        };

    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(readerFunc, i + 100));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(successCount, numThreads);
}

TEST_F(FileManagerTest, ConcurrentWriters) {
    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;

    auto writerFunc = [this, &successCount, &mtx, &cv, &ready](int clientId, int empId) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&ready] { return ready; });
        }

        if (fileManager->acquireWriteLock(empId, clientId)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            Employee emp;
            if (fileManager->getEmployee(empId, emp)) {
                emp.hours += 1.0;
                if (fileManager->updateEmployee(emp)) {
                    successCount++;
                }
            }

            fileManager->releaseLock(empId, clientId);
        }
        };

    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(writerFunc, i + 100, i + 1));
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        cv.notify_all();
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(successCount, numThreads);

    Employee emp;
    ASSERT_TRUE(fileManager->getEmployee(1, emp));
    EXPECT_DOUBLE_EQ(emp.hours, 41.0);

    ASSERT_TRUE(fileManager->getEmployee(2, emp));
    EXPECT_DOUBLE_EQ(emp.hours, 36.5);
}

TEST_F(FileManagerTest, ReadersBlockedByWriter) {
    std::vector<std::thread> threads;
    std::atomic<int> readerSuccessCount(0);
    std::mutex mtx;
    std::condition_variable cv;
    bool writerDone = false;

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 100));

    auto readerFunc = [this, &readerSuccessCount, &mtx, &cv, &writerDone](int clientId) {
        if (fileManager->acquireReadLock(1, clientId)) {
            readerSuccessCount++;
            fileManager->releaseLock(1, clientId);
        }
        else {
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&writerDone] { return writerDone; });
            }

            if (fileManager->acquireReadLock(1, clientId)) {
                readerSuccessCount++;
                fileManager->releaseLock(1, clientId);
            }
        }
        };

    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(readerFunc, i + 101));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(readerSuccessCount, 0);

    fileManager->releaseLock(1, 100);

    {
        std::lock_guard<std::mutex> lock(mtx);
        writerDone = true;
        cv.notify_all();
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(readerSuccessCount, 5);
}

TEST_F(FileManagerTest, WriterBlockedByReaders) {
    std::vector<std::thread> readerThreads;
    std::atomic<int> readerSuccessCount(0);
    std::atomic<bool> writerSuccess(false);
    std::mutex mtx;
    std::condition_variable cv;
    bool readersDone = false;

    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 100));

    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 102);
    fileManager->releaseLock(1, 103);

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 100));
    fileManager->releaseLock(1, 100);

    auto readerFunc = [this, &readerSuccessCount, &mtx, &cv, &readersDone](int clientId) {
        if (fileManager->acquireReadLock(1, clientId)) {
            readerSuccessCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            fileManager->releaseLock(1, clientId);

            if (readerSuccessCount == 3) {
                std::lock_guard<std::mutex> lock(mtx);
                readersDone = true;
                cv.notify_one();
            }
        }
        };

    auto writerFunc = [this, &writerSuccess, &mtx, &cv, &readersDone](int clientId) {
        if (fileManager->acquireWriteLock(1, clientId)) {
            writerSuccess = true;
            fileManager->releaseLock(1, clientId);
        }
        else {
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&readersDone] { return readersDone; });
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            if (fileManager->acquireWriteLock(1, clientId)) {
                writerSuccess = true;
                fileManager->releaseLock(1, clientId);
            }
        }
        };

    for (int i = 0; i < 3; ++i) {
        readerThreads.push_back(std::thread(readerFunc, i + 201));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::thread writerThread(writerFunc, 200);

    for (auto& thread : readerThreads) {
        thread.join();
    }
    writerThread.join();

    EXPECT_EQ(readerSuccessCount, 3);
    EXPECT_TRUE(writerSuccess);
}

TEST_F(FileManagerTest, EdgeCases) {
    ASSERT_TRUE(fileManager->acquireReadLock(999, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(999, 102));
    fileManager->releaseLock(999, 101);
    fileManager->releaseLock(999, 102);

    fileManager->releaseLock(888, 101);

    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    fileManager->releaseLock(1, 102);
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 101);

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 102);

    ASSERT_TRUE(fileManager->acquireWriteLock(0, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(0, 102));
    fileManager->releaseLock(0, 101);

    ASSERT_TRUE(fileManager->acquireWriteLock(-1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(-1, 102));
    fileManager->releaseLock(-1, 101);
}

TEST_F(FileManagerTest, MultipleReleases) {
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 101);

    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 102);
}

TEST_F(FileManagerTest, ComplexScenarios) {
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> empDist(1, 5);
    std::uniform_int_distribution<> opDist(0, 1);

    auto workerFunc = [this, &successCount, &empDist, &opDist, &gen](int clientId) {
        for (int i = 0; i < 10; ++i) {
            int empId = empDist(gen);
            bool isWrite = opDist(gen) == 1;

            if (isWrite) {
                if (fileManager->acquireWriteLock(empId, clientId)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    Employee emp;
                    if (fileManager->getEmployee(empId, emp)) {
                        emp.hours += 0.1;
                        if (fileManager->updateEmployee(emp)) {
                            successCount++;
                        }
                    }

                    fileManager->releaseLock(empId, clientId);
                }
            }
            else {
                if (fileManager->acquireReadLock(empId, clientId)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));

                    Employee emp;
                    if (fileManager->getEmployee(empId, emp)) {
                        successCount++;
                    }

                    fileManager->releaseLock(empId, clientId);
                }
            }
        }
        };

    for (int i = 0; i < 20; ++i) {
        threads.push_back(std::thread(workerFunc, i + 100));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_GT(successCount, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}