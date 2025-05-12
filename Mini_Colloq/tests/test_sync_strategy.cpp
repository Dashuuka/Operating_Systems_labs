#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include "SyncStrategy.h"
#include "Folder.h"
#include "File.h"

class Counter {
private:
    int value = 0;
public:
    void increment() { ++value; }
    int getValue() const { return value; }
};

class SyncStrategyTest : public ::testing::Test {};

TEST_F(SyncStrategyTest, NoSyncAllowsRaceCondition) {
    using UnsafeCounter = Synchronizable<Counter, NoSync>;
    UnsafeCounter counter;

    const int numThreads = 10;
    const int incrementsPerThread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&counter, incrementsPerThread]() {
            for (int j = 0; j < incrementsPerThread; ++j) {
                counter.increment();
            }
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    int expected = numThreads * incrementsPerThread;
    int actual = counter.getValue();

    std::cout << "NoSync: Expected " << expected << ", got " << actual << std::endl;
}

TEST_F(SyncStrategyTest, MutexSyncPreventsRaceCondition) {
    using SafeCounter = Synchronizable<Counter, MutexSync>;
    SafeCounter counter;

    const int numThreads = 10;
    const int incrementsPerThread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&counter, incrementsPerThread]() {
            for (int j = 0; j < incrementsPerThread; ++j) {
                auto lock = counter.getLock();
                counter.increment();
            }
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter.getValue(), numThreads * incrementsPerThread);
}

TEST_F(SyncStrategyTest, SharedLockAllowsConcurrentReads) {
    using SafeFolder = Synchronizable<Folder, MutexSync>;
    SafeFolder folder("TestFolder");

    {
        auto lock = folder.getLock();
        for (int i = 0; i < 100; ++i) {
            folder.addChild(std::make_shared<File>("file" + std::to_string(i),
                100, FileType::TEXT));
        }
    }

    const int numReaders = 5;
    std::vector<std::thread> readers;
    std::atomic<size_t> totalSize(0);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numReaders; ++i) {
        readers.emplace_back([&folder, &totalSize]() {
            auto sharedLock = folder.getSharedLock();
            size_t size = folder.getSize();
            totalSize += size;
            });
    }

    for (auto& t : readers) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(totalSize.load(), static_cast<size_t>(numReaders * 100 * 100));

    EXPECT_LT(duration.count(), 100);
}

TEST_F(SyncStrategyTest, ExclusiveLockBlocksReaders) {
    using SafeFolder = Synchronizable<Folder, MutexSync>;
    SafeFolder folder("TestFolder");

    std::atomic<bool> writerStarted(false);
    std::atomic<bool> writerFinished(false);
    std::atomic<bool> readerBlocked(true);

    std::thread writer([&folder, &writerStarted, &writerFinished]() {
        auto lock = folder.getLock();
        writerStarted = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        folder.addChild(std::make_shared<File>("newfile", 100, FileType::TEXT));
        writerFinished = true;
        });

    while (!writerStarted) {
        std::this_thread::yield();
    }

    std::thread reader([&folder, &readerBlocked, &writerFinished]() {
        auto sharedLock = folder.getSharedLock();
        readerBlocked = writerFinished.load();
        folder.getSize();
        });

    writer.join();
    reader.join();

    EXPECT_TRUE(readerBlocked);
}


TEST_F(SyncStrategyTest, ScopedLockRAII) {
    using SafeCounter = Synchronizable<Counter, MutexSync>;
    SafeCounter counter;

    {
        auto lock = counter.getLock();
        counter.increment();
        // lock уничтожается здесь
    }

    {
        auto lock = counter.getLock();
        counter.increment();
    }

    EXPECT_EQ(counter.getValue(), 2);
}

TEST_F(SyncStrategyTest, NoSyncBasicOperations) {
    using UnsafeFolder = Synchronizable<Folder, NoSync>;
    UnsafeFolder folder("TestFolder");

    folder.addChild(std::make_shared<File>("test.txt", 100, FileType::TEXT));
    EXPECT_EQ(folder.getChildren().size(), 1);
    EXPECT_EQ(folder.getSize(), 100);
}

TEST_F(SyncStrategyTest, MutexSyncBasicOperations) {
    using SafeFolder = Synchronizable<Folder, MutexSync>;
    SafeFolder folder("TestFolder");

    folder.addChild(std::make_shared<File>("test.txt", 100, FileType::TEXT));
    EXPECT_EQ(folder.getChildren().size(), 1);
    EXPECT_EQ(folder.getSize(), 100);
}