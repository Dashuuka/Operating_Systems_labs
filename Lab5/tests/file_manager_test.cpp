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
        // Создаем тестовый файл с данными
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
        // Удаляем тестовый файл
        remove("test_employees.dat");
    }

    // Вспомогательные функции для тестов
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

// Тесты для создания и чтения файла
TEST_F(FileManagerTest, CreateFile) {
    // Файл должен существовать после создания
    ASSERT_TRUE(fileExists("test_employees.dat"));

    // Размер файла должен соответствовать количеству записей
    EXPECT_EQ(getFileSize("test_employees.dat"), testEmployees.size() * sizeof(Employee));

    // Проверяем содержимое файла
    std::vector<Employee> fileEmployees = readEmployeesFromFile("test_employees.dat");
    ASSERT_EQ(fileEmployees.size(), testEmployees.size());

    for (size_t i = 0; i < testEmployees.size(); ++i) {
        EXPECT_EQ(fileEmployees[i].num, testEmployees[i].num);
        EXPECT_STREQ(fileEmployees[i].name, testEmployees[i].name);
        EXPECT_DOUBLE_EQ(fileEmployees[i].hours, testEmployees[i].hours);
    }
}

TEST_F(FileManagerTest, CreateEmptyFile) {
    // Создаем пустой файл
    FileManager emptyManager("empty_employees.dat");
    std::vector<Employee> emptyEmployees;
    ASSERT_TRUE(emptyManager.createFile(emptyEmployees));

    // Файл должен существовать, но быть пустым
    ASSERT_TRUE(fileExists("empty_employees.dat"));
    EXPECT_EQ(getFileSize("empty_employees.dat"), 0);

    // Удаляем тестовый файл
    remove("empty_employees.dat");
}

TEST_F(FileManagerTest, CreateFileWithMaxRecords) {
    // Создаем файл с большим количеством записей (100)
    std::vector<Employee> manyEmployees;
    for (int i = 1; i <= 100; ++i) {
        manyEmployees.push_back(Employee(i, "Employee" + std::to_string(i), 40.0 + i / 10.0));
    }

    FileManager largeManager("large_employees.dat");
    ASSERT_TRUE(largeManager.createFile(manyEmployees));

    // Проверяем размер файла
    ASSERT_TRUE(fileExists("large_employees.dat"));
    EXPECT_EQ(getFileSize("large_employees.dat"), manyEmployees.size() * sizeof(Employee));

    // Удаляем тестовый файл
    remove("large_employees.dat");
}

// Тесты для получения сотрудника
TEST_F(FileManagerTest, GetEmployeeById) {
    Employee emp;

    // Проверяем, что можем получить существующего сотрудника
    ASSERT_TRUE(fileManager->getEmployee(1, emp));
    EXPECT_EQ(emp.num, 1);
    EXPECT_STREQ(emp.name, "Alice");
    EXPECT_DOUBLE_EQ(emp.hours, 40.0);

    // Проверяем другого сотрудника
    ASSERT_TRUE(fileManager->getEmployee(3, emp));
    EXPECT_EQ(emp.num, 3);
    EXPECT_STREQ(emp.name, "Charlie");
    EXPECT_DOUBLE_EQ(emp.hours, 42.0);

    // Проверяем, что получаем false для несуществующего сотрудника
    ASSERT_FALSE(fileManager->getEmployee(999, emp));

    // Проверяем граничные случаи
    ASSERT_FALSE(fileManager->getEmployee(0, emp));
    ASSERT_FALSE(fileManager->getEmployee(-1, emp));

    // Проверяем первого и последнего сотрудника
    ASSERT_TRUE(fileManager->getEmployee(1, emp));
    EXPECT_EQ(emp.num, 1);

    ASSERT_TRUE(fileManager->getEmployee(5, emp));
    EXPECT_EQ(emp.num, 5);
    EXPECT_STREQ(emp.name, "Emma");
}

// Тесты для обновления сотрудника
TEST_F(FileManagerTest, UpdateEmployee) {
    // Обновляем существующего сотрудника
    Employee emp(2, "Bobby", 38.0);
    ASSERT_TRUE(fileManager->updateEmployee(emp));

    // Проверяем, что данные обновились
    Employee updatedEmp;
    ASSERT_TRUE(fileManager->getEmployee(2, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 2);
    EXPECT_STREQ(updatedEmp.name, "Bobby");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 38.0);

    // Обновляем еще одного сотрудника
    Employee emp3(3, "Charles", 43.5);
    ASSERT_TRUE(fileManager->updateEmployee(emp3));

    // Проверяем, что данные обновились
    ASSERT_TRUE(fileManager->getEmployee(3, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 3);
    EXPECT_STREQ(updatedEmp.name, "Charles");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 43.5);

    // Проверяем, что невозможно обновить несуществующего сотрудника
    Employee nonExistentEmp(999, "Nobody", 0.0);
    ASSERT_FALSE(fileManager->updateEmployee(nonExistentEmp));

    // Проверяем, что первый сотрудник не изменился
    ASSERT_TRUE(fileManager->getEmployee(1, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 1);
    EXPECT_STREQ(updatedEmp.name, "Alice");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 40.0);

    // Обновление с пустым именем
    Employee emp4(4, "", 39.0);
    ASSERT_TRUE(fileManager->updateEmployee(emp4));

    // Проверяем, что имя стало пустым
    ASSERT_TRUE(fileManager->getEmployee(4, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 4);
    EXPECT_STREQ(updatedEmp.name, "");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 39.0);

    // Обновление только часов, не меняя имя
    Employee emp5 = testEmployees[4]; // Emma
    emp5.hours = 46.0;
    ASSERT_TRUE(fileManager->updateEmployee(emp5));

    // Проверяем, что только часы изменились
    ASSERT_TRUE(fileManager->getEmployee(5, updatedEmp));
    EXPECT_EQ(updatedEmp.num, 5);
    EXPECT_STREQ(updatedEmp.name, "Emma");
    EXPECT_DOUBLE_EQ(updatedEmp.hours, 46.0);
}

// Тесты для блокировок на чтение
TEST_F(FileManagerTest, ReadLocks) {
    // Первый клиент получает блокировку на чтение
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));

    // Второй клиент тоже может получить блокировку на чтение
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));

    // Третий клиент тоже может получить блокировку на чтение
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    // Освобождаем блокировки в разном порядке
    fileManager->releaseLock(1, 102);
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 103);

    // После освобождения все снова могут получить блокировки
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 102);

    // Проверяем для другой записи
    ASSERT_TRUE(fileManager->acquireReadLock(2, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(2, 102));
    fileManager->releaseLock(2, 101);
    fileManager->releaseLock(2, 102);

    // Проверяем, что блокировки на разные записи не влияют друг на друга
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(2, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(3, 103));
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(2, 102);
    fileManager->releaseLock(3, 103);
}

// Тесты для блокировок на запись
TEST_F(FileManagerTest, WriteLocks) {
    // Первый клиент получает блокировку на запись
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 101));

    // Второй клиент не может получить блокировку ни на чтение, ни на запись
    ASSERT_FALSE(fileManager->acquireReadLock(1, 102));
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    // Это не влияет на блокировки для других записей
    ASSERT_TRUE(fileManager->acquireWriteLock(2, 102));

    // Освобождаем блокировки
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(2, 102);

    // Теперь второй клиент может получить блокировку
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));

    // Первый клиент не может получить блокировку
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));

    fileManager->releaseLock(1, 102);

    // Если клиент запрашивает блокировку на запись, а потом еще раз на чтение,
    // то он не может получить вторую блокировку (уже имеет блокировку)
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));

    fileManager->releaseLock(1, 101);

    // Проверяем для несуществующей записи
    ASSERT_TRUE(fileManager->acquireWriteLock(999, 101));
    ASSERT_FALSE(fileManager->acquireWriteLock(999, 102));
    fileManager->releaseLock(999, 101);
}

// Тесты для взаимодействия читателей и писателей
TEST_F(FileManagerTest, ReadWriteContention) {
    // Читатель получает блокировку
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));

    // Писатель не может получить блокировку
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    // Еще один читатель может получить блокировку
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    // Третий читатель тоже может получить блокировку
    ASSERT_TRUE(fileManager->acquireReadLock(1, 104));

    // Освобождаем блокировки читателей по одной и проверяем,
    // что писатель все еще не может получить блокировку
    fileManager->releaseLock(1, 101);
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    fileManager->releaseLock(1, 103);
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));

    // Освобождаем последнего читателя
    fileManager->releaseLock(1, 104);

    // Теперь писатель может получить блокировку
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));

    // Ни один читатель не может получить блокировку
    ASSERT_FALSE(fileManager->acquireReadLock(1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(1, 103));

    // Освобождаем блокировку писателя
    fileManager->releaseLock(1, 102);

    // Теперь читатели могут получить блокировку
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 103);
}

// Тесты на многопоточность
TEST_F(FileManagerTest, ConcurrentReaders) {
    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);

    auto readerFunc = [this, &successCount](int clientId) {
        // Каждый поток пытается получить блокировку на чтение для записи 1
        if (fileManager->acquireReadLock(1, clientId)) {
            // Имитируем чтение с задержкой
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Проверяем, что можем прочитать запись
            Employee emp;
            if (fileManager->getEmployee(1, emp) && emp.num == 1) {
                successCount++;
            }

            // Освобождаем блокировку
            fileManager->releaseLock(1, clientId);
        }
        };

    // Запускаем потоки
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(readerFunc, i + 100));
    }

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Все потоки должны успешно получить блокировку и прочитать запись
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
        // Ждем сигнала, чтобы все потоки начали одновременно
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&ready] { return ready; });
        }

        // Пытаемся получить блокировку на запись
        if (fileManager->acquireWriteLock(empId, clientId)) {
            // Имитируем модификацию с задержкой
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Читаем запись
            Employee emp;
            if (fileManager->getEmployee(empId, emp)) {
                // Изменяем запись
                emp.hours += 1.0;
                // Записываем обратно
                if (fileManager->updateEmployee(emp)) {
                    successCount++;
                }
            }

            // Освобождаем блокировку
            fileManager->releaseLock(empId, clientId);
        }
        };

    // Запускаем потоки, каждый для своей записи
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(writerFunc, i + 100, i + 1));
    }

    // Даем сигнал всем потокам начать
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        cv.notify_all();
    }

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Все потоки должны успешно получить блокировку и обновить запись
    EXPECT_EQ(successCount, numThreads);

    // Проверяем, что часы увеличились на 1
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

    // Сначала получаем блокировку на запись
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 100));

    // Функция читателя
    auto readerFunc = [this, &readerSuccessCount, &mtx, &cv, &writerDone](int clientId) {
        // Пытаемся получить блокировку на чтение
        if (fileManager->acquireReadLock(1, clientId)) {
            // Если получили, увеличиваем счетчик
            readerSuccessCount++;

            // Освобождаем блокировку
            fileManager->releaseLock(1, clientId);
        }
        else {
            // Если не получили, ждем сигнала от писателя и пробуем снова
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&writerDone] { return writerDone; });
            }

            // Теперь должны получить блокировку
            if (fileManager->acquireReadLock(1, clientId)) {
                readerSuccessCount++;
                fileManager->releaseLock(1, clientId);
            }
        }
        };

    // Запускаем потоки читателей
    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(readerFunc, i + 101));
    }

    // Даем время потокам попытаться получить блокировку
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // На этом этапе ни один читатель не должен получить блокировку
    EXPECT_EQ(readerSuccessCount, 0);

    // Освобождаем блокировку писателя
    fileManager->releaseLock(1, 100);

    // Сигнализируем потокам, что писатель закончил
    {
        std::lock_guard<std::mutex> lock(mtx);
        writerDone = true;
        cv.notify_all();
    }

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Теперь все читатели должны получить блокировку
    EXPECT_EQ(readerSuccessCount, 5);
}

TEST_F(FileManagerTest, WriterBlockedByReaders) {
    std::vector<std::thread> readerThreads;
    std::atomic<int> readerSuccessCount(0);
    std::atomic<bool> writerSuccess(false);
    std::mutex mtx;
    std::condition_variable cv;
    bool readersDone = false;

    // Сначала получаем блокировки на чтение
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 102));
    ASSERT_TRUE(fileManager->acquireReadLock(1, 103));

    // Писатель не должен получить блокировку
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 100));

    // Освобождаем все блокировки чтения
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 102);
    fileManager->releaseLock(1, 103);

    // Теперь писатель должен получить блокировку
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 100));

    // Освобождаем блокировку писателя
    fileManager->releaseLock(1, 100);

    // Теперь можно использовать многопоточный тест
    auto readerFunc = [this, &readerSuccessCount, &mtx, &cv, &readersDone](int clientId) {
        // Получаем блокировку на чтение
        if (fileManager->acquireReadLock(1, clientId)) {
            readerSuccessCount++;

            // Читатели держат блокировку некоторое время
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Освобождаем блокировку
            fileManager->releaseLock(1, clientId);

            // Если это последний читатель, сигнализируем
            if (readerSuccessCount == 3) {
                std::lock_guard<std::mutex> lock(mtx);
                readersDone = true;
                cv.notify_one();
            }
        }
        };

    // Функция писателя
    auto writerFunc = [this, &writerSuccess, &mtx, &cv, &readersDone](int clientId) {
        // Пытаемся получить блокировку на запись
        if (fileManager->acquireWriteLock(1, clientId)) {
            writerSuccess = true;
            fileManager->releaseLock(1, clientId);
        }
        else {
            // Ждем, пока читатели закончат
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&readersDone] { return readersDone; });
            }

            // Небольшая задержка, чтобы дать время на реальное освобождение блокировок
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            // Теперь должны получить блокировку
            if (fileManager->acquireWriteLock(1, clientId)) {
                writerSuccess = true;
                fileManager->releaseLock(1, clientId);
            }
        }
        };

    // Запускаем потоки читателей
    for (int i = 0; i < 3; ++i) {
        readerThreads.push_back(std::thread(readerFunc, i + 201));
    }

    // Даем время читателям получить блокировку
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // Запускаем поток писателя
    std::thread writerThread(writerFunc, 200);

    // Ждем завершения всех потоков
    for (auto& thread : readerThreads) {
        thread.join();
    }
    writerThread.join();

    // Все читатели должны получить блокировку
    EXPECT_EQ(readerSuccessCount, 3);

    // Писатель должен получить блокировку после завершения читателей
    EXPECT_TRUE(writerSuccess);
}
// Тесты на краевые случаи
TEST_F(FileManagerTest, EdgeCases) {
    // Пытаемся получить блокировки на несуществующие записи
    ASSERT_TRUE(fileManager->acquireReadLock(999, 101));
    ASSERT_TRUE(fileManager->acquireReadLock(999, 102));
    fileManager->releaseLock(999, 101);
    fileManager->releaseLock(999, 102);

    // Пытаемся освободить несуществующую блокировку
    fileManager->releaseLock(888, 101); // Должно просто ничего не делать

    // Пытаемся освободить блокировку для неправильного клиента
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));
    fileManager->releaseLock(1, 102); // Не должно освободить блокировку 101
    // Блокировка все еще должна существовать
    ASSERT_FALSE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 101);

    // Теперь блокировка освобождена
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 102);

    // Проверяем, что блокировка для ID 0 (невалидный) тоже работает
    ASSERT_TRUE(fileManager->acquireWriteLock(0, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(0, 102));
    fileManager->releaseLock(0, 101);

    // Проверяем, что блокировка для отрицательного ID тоже работает
    ASSERT_TRUE(fileManager->acquireWriteLock(-1, 101));
    ASSERT_FALSE(fileManager->acquireReadLock(-1, 102));
    fileManager->releaseLock(-1, 101);
}

// Тесты на множественные освобождения
TEST_F(FileManagerTest, MultipleReleases) {
    // Получаем блокировку
    ASSERT_TRUE(fileManager->acquireReadLock(1, 101));

    // Освобождаем ее дважды
    fileManager->releaseLock(1, 101);
    fileManager->releaseLock(1, 101); // Повторное освобождение не должно вызывать ошибок

    // Теперь должны иметь возможность получить блокировку на запись
    ASSERT_TRUE(fileManager->acquireWriteLock(1, 102));
    fileManager->releaseLock(1, 102);
}

// Тесты на сложные сценарии с многими записями и клиентами
// Тест на сложные сценарии с многими записями и клиентами
TEST_F(FileManagerTest, ComplexScenarios) {
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> empDist(1, 5);  // ID сотрудников от 1 до 5
    std::uniform_int_distribution<> opDist(0, 1);   // 0 = чтение, 1 = запись

    // Добавляем gen в список захвата
    auto workerFunc = [this, &successCount, &empDist, &opDist, &gen](int clientId) {
        for (int i = 0; i < 10; ++i) {
            int empId = empDist(gen);
            bool isWrite = opDist(gen) == 1;

            if (isWrite) {
                if (fileManager->acquireWriteLock(empId, clientId)) {
                    // Имитация задержки
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
                    // Имитация задержки
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

    // Запускаем 20 потоков, каждый выполнит 10 операций
    for (int i = 0; i < 20; ++i) {
        threads.push_back(std::thread(workerFunc, i + 100));
    }

    // Ждем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Должны быть успешные операции
    EXPECT_GT(successCount, 0);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}