#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include "ConcreteVisitors.h"
#include "SyncStrategy.h"

class MultithreadingTest : public ::testing::Test {
protected:
    std::shared_ptr<Disk> createLargeFileSystem() {
        auto disk = std::make_shared<Disk>("D:", 5000000000);

        for (int i = 0; i < 10; ++i) {
            auto folder = std::make_shared<Folder>("folder" + std::to_string(i));
            for (int j = 0; j < 100; ++j) {
                folder->addChild(std::make_shared<File>(
                    "file" + std::to_string(j) + ".txt",
                    1024,
                    FileType::TEXT
                ));
            }
            disk->addChild(folder);
        }

        return disk;
    }
};

TEST_F(MultithreadingTest, ConcurrentFileCount) {
    auto disk = createLargeFileSystem();
    std::atomic<int> totalVisits(0);

    const int numThreads = 4;
    std::vector<std::thread> threads;
    std::vector<FileCounterVisitor> visitors(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&disk, &visitors, i]() {
            disk->accept(visitors[i]);
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (const auto& visitor : visitors) {
        EXPECT_EQ(visitor.getTextFileCount(), 1000);
    }
}

TEST_F(MultithreadingTest, ThreadSafeFolder) {
    using SafeFolder = Synchronizable<Folder, MutexSync>;
    auto folder = std::make_shared<SafeFolder>("safe_folder");

    const int numThreads = 10;
    const int filesPerThread = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([folder, i, filesPerThread]() {
            for (int j = 0; j < filesPerThread; ++j) {
                auto lock = folder->getLock();
                folder->addChild(std::make_shared<File>(
                    "file_" + std::to_string(i) + "_" + std::to_string(j),
                    100,
                    FileType::TEXT
                ));
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(folder->getChildren().size(), numThreads * filesPerThread);
}

TEST_F(MultithreadingTest, ConcurrentTraversal) {
    auto disk = createLargeFileSystem();
    const int numThreads = 8;
    std::vector<std::thread> threads;
    std::atomic<int> completedTraversals(0);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&disk, &completedTraversals]() {
            SizeCalculatorVisitor visitor;
            disk->accept(visitor);
            EXPECT_GT(visitor.getTotalSize(), 0);
            completedTraversals++;
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(completedTraversals, numThreads);
}