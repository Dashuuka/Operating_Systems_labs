#include <gtest/gtest.h>
#include <thread>
#include <future>
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include "ConcreteVisitors.h"
#include "SyncStrategy.h"

class IntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<Disk> createComplexFileSystem() {
        auto disk = std::make_shared<Disk>("MainDisk", 1000000000);

        auto root = std::make_shared<Folder>("root");
        auto system = std::make_shared<Folder>("System");
        auto users = std::make_shared<Folder>("Users");
        auto programs = std::make_shared<Folder>("Programs");

        system->addChild(std::make_shared<File>("kernel.sys", 10485760, FileType::BINARY));
        system->addChild(std::make_shared<File>("config.ini", 1024, FileType::TEXT));

        for (int i = 0; i < 3; ++i) {
            auto userDir = std::make_shared<Folder>("User" + std::to_string(i));
            auto docs = std::make_shared<Folder>("Documents");
            auto downloads = std::make_shared<Folder>("Downloads");

            for (int j = 0; j < 10; ++j) {
                docs->addChild(std::make_shared<File>("doc" + std::to_string(j) + ".txt",
                    2048, FileType::TEXT));
                downloads->addChild(std::make_shared<File>("file" + std::to_string(j) + ".bin",
                    8192, FileType::BINARY));
            }

            userDir->addChild(docs);
            userDir->addChild(downloads);
            users->addChild(userDir);
        }

        programs->addChild(std::make_shared<File>("chrome.exe", 52428800, FileType::EXECUTABLE));
        programs->addChild(std::make_shared<File>("notepad.exe", 1048576, FileType::EXECUTABLE));
        programs->addChild(std::make_shared<File>("game.exe", 104857600, FileType::EXECUTABLE));

        root->addChild(system);
        root->addChild(users);
        root->addChild(programs);
        disk->addChild(root);

        return disk;
    }
};

TEST_F(IntegrationTest, FullSystemTraversal) {
    auto disk = createComplexFileSystem();

    PrintVisitor printer;
    SizeCalculatorVisitor sizeCalc;
    FileCounterVisitor counter;

    disk->accept(printer);
    disk->accept(sizeCalc);
    disk->accept(counter);

    EXPECT_GT(sizeCalc.getTotalSize(), 0);
    EXPECT_EQ(counter.getTextFileCount(), 31);
    EXPECT_EQ(counter.getBinaryFileCount(), 31);
    EXPECT_EQ(counter.getExecutableFileCount(), 3);
}

TEST_F(IntegrationTest, ConcurrentVisitors) {
    auto disk = createComplexFileSystem();

    std::vector<std::future<size_t>> sizeFutures;
    std::vector<std::future<int>> countFutures;

    for (int i = 0; i < 5; ++i) {
        sizeFutures.push_back(std::async(std::launch::async, [&disk]() {
            SizeCalculatorVisitor visitor;
            disk->accept(visitor);
            return visitor.getTotalSize();
            }));

        countFutures.push_back(std::async(std::launch::async, [&disk]() {
            FileCounterVisitor visitor;
            disk->accept(visitor);
            return visitor.getTextFileCount();
            }));
    }

    size_t expectedSize = sizeFutures[0].get();
    int expectedCount = countFutures[0].get();

    for (size_t i = 1; i < sizeFutures.size(); ++i) {
        EXPECT_EQ(sizeFutures[i].get(), expectedSize);
        EXPECT_EQ(countFutures[i].get(), expectedCount);
    }
}

TEST_F(IntegrationTest, DynamicFileSystemModification) {
    using SafeDisk = Synchronizable<Disk, MutexSync>;
    using SafeFolder = Synchronizable<Folder, MutexSync>;

    auto disk = std::make_shared<SafeDisk>("DynamicDisk", 1000000000);
    auto root = std::make_shared<SafeFolder>("root");

    {
        auto lock = disk->getLock();
        disk->addChild(root);
    }

    const int numThreads = 4;
    const int filesPerThread = 25;
    std::vector<std::thread> writers;

    for (int i = 0; i < numThreads; ++i) {
        writers.emplace_back([root, i, filesPerThread]() {
            for (int j = 0; j < filesPerThread; ++j) {
                auto lock = root->getLock();
                root->addChild(std::make_shared<File>(
                    "thread" + std::to_string(i) + "_file" + std::to_string(j) + ".txt",
                    1024,
                    FileType::TEXT
                ));
            }
            });
    }

    std::vector<std::thread> readers;
    std::atomic<int> totalReads(0);

    for (int i = 0; i < numThreads; ++i) {
        readers.emplace_back([root, &totalReads]() {
            for (int j = 0; j < 100; ++j) {
                auto lock = root->getSharedLock();
                size_t size = root->getChildren().size();
                totalReads++;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
            });
    }

    for (auto& t : writers) t.join();
    for (auto& t : readers) t.join();

    {
        auto lock = root->getSharedLock();
        EXPECT_EQ(root->getChildren().size(), numThreads * filesPerThread);
    }

    EXPECT_EQ(totalReads, numThreads * 100);
}

TEST_F(IntegrationTest, VisitorChaining) {
    auto disk = createComplexFileSystem();

    class ChainedVisitor : public FileSystemVisitor {
    private:
        std::vector<std::unique_ptr<FileSystemVisitor>> visitors;

    public:
        void addVisitor(std::unique_ptr<FileSystemVisitor> visitor) {
            visitors.push_back(std::move(visitor));
        }

        void visitDisk(Disk& disk) override {
            for (auto& visitor : visitors) {
                visitor->visitDisk(disk);
            }
            for (auto& child : disk.getChildren()) {
                child->accept(*this);
            }
        }

        void visitFolder(Folder& folder) override {
            for (auto& visitor : visitors) {
                visitor->visitFolder(folder);
            }
            for (auto& child : folder.getChildren()) {
                child->accept(*this);
            }
        }

        void visitFile(File& file) override {
            for (auto& visitor : visitors) {
                visitor->visitFile(file);
            }
        }
    };

    ChainedVisitor chained;
    chained.addVisitor(std::make_unique<PrintVisitor>());
    chained.addVisitor(std::make_unique<SizeCalculatorVisitor>());
    chained.addVisitor(std::make_unique<FileCounterVisitor>());

    disk->accept(chained);

    SUCCEED();
}

TEST_F(IntegrationTest, ErrorHandling) {
    auto disk = std::make_shared<Disk>("TestDisk", 1000);

    disk->addChild(nullptr);
    disk->addChild(std::make_shared<File>("normal.txt", 100, FileType::TEXT));

    SizeCalculatorVisitor sizeCalc;
    EXPECT_NO_THROW(disk->accept(sizeCalc));
    EXPECT_EQ(sizeCalc.getTotalSize(), 100);

    FileCounterVisitor counter;
    EXPECT_NO_THROW(disk->accept(counter));
    EXPECT_EQ(counter.getTextFileCount(), 1);

    class ThrowingVisitor : public FileSystemVisitor {
    public:
        void visitDisk(Disk& disk) override {
            throw std::runtime_error("Disk error");
        }
        void visitFolder(Folder& folder) override {
            throw std::runtime_error("Folder error");
        }
        void visitFile(File& file) override {
            throw std::runtime_error("File error");
        }
    };

    ThrowingVisitor thrower;
    EXPECT_THROW(disk->accept(thrower), std::runtime_error);
}