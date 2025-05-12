#include <gtest/gtest.h>
#include <chrono>
#include <random>
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include "ConcreteVisitors.h"
#include "SyncStrategy.h"

class PerformanceTest : public ::testing::Test {
protected:
    std::shared_ptr<Disk> createLargeFileSystem(int numFolders, int filesPerFolder) {
        auto disk = std::make_shared<Disk>("PerfDisk", 1000000000);
        auto root = std::make_shared<Folder>("root");

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> sizeDist(1024, 1048576);
        std::uniform_int_distribution<> typeDist(0, 2);

        for (int i = 0; i < numFolders; ++i) {
            auto folder = std::make_shared<Folder>("folder" + std::to_string(i));

            for (int j = 0; j < filesPerFolder; ++j) {
                FileType type = static_cast<FileType>(typeDist(gen));
                size_t size = sizeDist(gen);
                folder->addChild(std::make_shared<File>(
                    "file" + std::to_string(j), size, type));
            }

            root->addChild(folder);
        }

        disk->addChild(root);
        return disk;
    }

    template<typename Func>
    long long measureTime(Func func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
};

TEST_F(PerformanceTest, TraversalScaling) {
    std::vector<int> sizes = { 10, 100, 1000 };
    std::vector<long long> times;

    for (int size : sizes) {
        auto disk = createLargeFileSystem(size, 100);

        auto time = measureTime([&disk]() {
            SizeCalculatorVisitor visitor;
            disk->accept(visitor);
            });

        times.push_back(time);
        std::cout << "Size " << size << ": " << time << " microseconds\n";
    }

    double ratio1 = static_cast<double>(times[1]) / times[0];
    double ratio2 = static_cast<double>(times[2]) / times[1];

    EXPECT_LT(std::abs(ratio1 - ratio2), ratio1 * 0.5);
}

TEST_F(PerformanceTest, ConcurrentVsSingleThreaded) {
    auto disk = createLargeFileSystem(100, 100);

    auto singleThreadTime = measureTime([&disk]() {
        FileCounterVisitor visitor;
        disk->accept(visitor);
        });

    const int numThreads = 4;
    auto multiThreadTime = measureTime([&disk, numThreads]() {
        std::vector<std::thread> threads;
        std::vector<FileCounterVisitor> visitors(numThreads);

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&disk, &visitors, i]() {
                disk->accept(visitors[i]);
                });
        }

        for (auto& t : threads) {
            t.join();
        }
        });

    std::cout << "Single-threaded: " << singleThreadTime << " microseconds\n";
    std::cout << "Multi-threaded (4 threads): " << multiThreadTime << " microseconds\n";

    EXPECT_LT(multiThreadTime, singleThreadTime * 10);
}
TEST_F(PerformanceTest, SyncOverhead) {
    using UnsafeFolder = Folder;
    using SafeFolder = Synchronizable<Folder, MutexSync>;

    const int numOperations = 10000;

    auto unsafeFolder = std::make_shared<UnsafeFolder>("Unsafe");
    auto unsafeTime = measureTime([&unsafeFolder, numOperations]() {
        for (int i = 0; i < numOperations; ++i) {
            unsafeFolder->addChild(std::make_shared<File>("file" + std::to_string(i),
                100, FileType::TEXT));
        }
        });

    auto safeFolder = std::make_shared<SafeFolder>("Safe");
    auto safeTime = measureTime([&safeFolder, numOperations]() {
        for (int i = 0; i < numOperations; ++i) {
            auto lock = safeFolder->getLock();
            safeFolder->addChild(std::make_shared<File>("file" + std::to_string(i),
                100, FileType::TEXT));
        }
        });

    std::cout << "Unsafe: " << unsafeTime << " microseconds\n";
    std::cout << "Safe: " << safeTime << " microseconds\n";

    double overhead = static_cast<double>(safeTime) / unsafeTime;
    EXPECT_LT(overhead, 10.0);
}

TEST_F(PerformanceTest, MemoryUsage) {
    const int numFiles = 100000;


    auto disk = std::make_shared<Disk>("MemTest", 1000000000);
    auto root = std::make_shared<Folder>("root");

    for (int i = 0; i < numFiles; ++i) {
        root->addChild(std::make_shared<File>("file" + std::to_string(i),
            1024, FileType::TEXT));
    }

    disk->addChild(root);

    EXPECT_EQ(root->getChildren().size(), numFiles);

    // В реальном тесте здесь бы проверялось конечное использование памяти
}

TEST_F(PerformanceTest, CacheEfficiency) {
    auto disk = createLargeFileSystem(10, 1000);

    auto coldCacheTime = measureTime([&disk]() {
        SizeCalculatorVisitor visitor;
        disk->accept(visitor);
        });

    std::vector<long long> hotCacheTimes;
    for (int i = 0; i < 5; ++i) {
        auto time = measureTime([&disk]() {
            SizeCalculatorVisitor visitor;
            disk->accept(visitor);
            });
        hotCacheTimes.push_back(time);
    }

    long long avgHotCacheTime = 0;
    for (auto time : hotCacheTimes) {
        avgHotCacheTime += time;
    }
    avgHotCacheTime /= hotCacheTimes.size();

    std::cout << "Cold cache: " << coldCacheTime << " microseconds\n";
    std::cout << "Hot cache (avg): " << avgHotCacheTime << " microseconds\n";

    EXPECT_LT(avgHotCacheTime, coldCacheTime);
}
