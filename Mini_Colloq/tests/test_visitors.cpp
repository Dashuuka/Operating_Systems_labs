#include <gtest/gtest.h>
#include <sstream>
#include <thread>
#include "ConcreteVisitors.h"
#include "Disk.h"
#include "Folder.h"
#include "File.h"

class VisitorsTest : public ::testing::Test {
protected:
    void SetUp() override {
        disk = std::make_shared<Disk>("TestDisk", 1000000);

        auto rootFolder = std::make_shared<Folder>("root");
        auto docsFolder = std::make_shared<Folder>("documents");
        auto progFolder = std::make_shared<Folder>("programs");

        docsFolder->addChild(std::make_shared<File>("report.txt", 1024, FileType::TEXT));
        docsFolder->addChild(std::make_shared<File>("data.bin", 2048, FileType::BINARY));
        docsFolder->addChild(std::make_shared<File>("readme.txt", 512, FileType::TEXT));

        progFolder->addChild(std::make_shared<File>("app.exe", 4096, FileType::EXECUTABLE));
        progFolder->addChild(std::make_shared<File>("lib.dll", 8192, FileType::BINARY));

        rootFolder->addChild(docsFolder);
        rootFolder->addChild(progFolder);
        rootFolder->addChild(std::make_shared<File>("config.ini", 256, FileType::TEXT));

        disk->addChild(rootFolder);
    }

    std::shared_ptr<Disk> disk;
};

class PrintVisitorTest : public VisitorsTest {
protected:
    void SetUp() override {
        VisitorsTest::SetUp();
        oldCoutStreamBuf = std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());
    }

    void TearDown() override {
        std::cout.rdbuf(oldCoutStreamBuf);
    }

    std::stringstream ss;
    std::streambuf* oldCoutStreamBuf;
};

TEST_F(PrintVisitorTest, PrintsDiskInfo) {
    PrintVisitor printer;
    disk->accept(printer);

    std::string output = ss.str();
    EXPECT_TRUE(output.find("[DISK] TestDisk") != std::string::npos);
    EXPECT_TRUE(output.find("Total: 1000000 bytes") != std::string::npos);
}

TEST_F(PrintVisitorTest, PrintsFolderHierarchy) {
    PrintVisitor printer;
    disk->accept(printer);

    std::string output = ss.str();
    EXPECT_TRUE(output.find("[FOLDER] root") != std::string::npos);
    EXPECT_TRUE(output.find("[FOLDER] documents") != std::string::npos);
    EXPECT_TRUE(output.find("[FOLDER] programs") != std::string::npos);
}

TEST_F(PrintVisitorTest, PrintsFileDetails) {
    PrintVisitor printer;
    disk->accept(printer);

    std::string output = ss.str();
    EXPECT_TRUE(output.find("[FILE] report.txt (TEXT, 1024 bytes)") != std::string::npos);
    EXPECT_TRUE(output.find("[FILE] app.exe (EXEC, 4096 bytes)") != std::string::npos);
    EXPECT_TRUE(output.find("[FILE] lib.dll (BINARY, 8192 bytes)") != std::string::npos);
}

TEST_F(PrintVisitorTest, ProperIndentation) {
    PrintVisitor printer;
    disk->accept(printer);

    std::string output = ss.str();
    EXPECT_TRUE(output.find("  [FOLDER] root") != std::string::npos);
    EXPECT_TRUE(output.find("    [FOLDER] documents") != std::string::npos);
    EXPECT_TRUE(output.find("      [FILE] report.txt") != std::string::npos);
}

TEST_F(VisitorsTest, SizeCalculatorCorrectTotal) {
    SizeCalculatorVisitor sizeCalc;
    disk->accept(sizeCalc);

    EXPECT_EQ(sizeCalc.getTotalSize(), 16128);
}

TEST_F(VisitorsTest, SizeCalculatorEmptyDisk) {
    auto emptyDisk = std::make_shared<Disk>("Empty", 1000);
    SizeCalculatorVisitor sizeCalc;
    emptyDisk->accept(sizeCalc);

    EXPECT_EQ(sizeCalc.getTotalSize(), 0);
}

TEST_F(VisitorsTest, SizeCalculatorSingleFile) {
    auto singleFileDisk = std::make_shared<Disk>("Single", 1000);
    singleFileDisk->addChild(std::make_shared<File>("only.txt", 42, FileType::TEXT));

    SizeCalculatorVisitor sizeCalc;
    singleFileDisk->accept(sizeCalc);

    EXPECT_EQ(sizeCalc.getTotalSize(), 42);
}

TEST_F(VisitorsTest, FileCounterCorrectCounts) {
    FileCounterVisitor counter;
    disk->accept(counter);

    EXPECT_EQ(counter.getTextFileCount(), 3);
    EXPECT_EQ(counter.getBinaryFileCount(), 2);
    EXPECT_EQ(counter.getExecutableFileCount(), 1);
}

TEST_F(VisitorsTest, FileCounterEmptyDisk) {
    auto emptyDisk = std::make_shared<Disk>("Empty", 1000);
    FileCounterVisitor counter;
    emptyDisk->accept(counter);

    EXPECT_EQ(counter.getTextFileCount(), 0);
    EXPECT_EQ(counter.getBinaryFileCount(), 0);
    EXPECT_EQ(counter.getExecutableFileCount(), 0);
}

TEST_F(VisitorsTest, FileCounterSingleType) {
    auto textOnlyDisk = std::make_shared<Disk>("TextOnly", 1000);
    auto folder = std::make_shared<Folder>("texts");

    for (int i = 0; i < 10; ++i) {
        folder->addChild(std::make_shared<File>("file" + std::to_string(i) + ".txt",
            100, FileType::TEXT));
    }

    textOnlyDisk->addChild(folder);

    FileCounterVisitor counter;
    textOnlyDisk->accept(counter);

    EXPECT_EQ(counter.getTextFileCount(), 10);
    EXPECT_EQ(counter.getBinaryFileCount(), 0);
    EXPECT_EQ(counter.getExecutableFileCount(), 0);
}

TEST_F(VisitorsTest, FileCounterThreadSafety) {
    auto bigDisk = std::make_shared<Disk>("BigDisk", 10000000);

    for (int i = 0; i < 10; ++i) {
        auto folder = std::make_shared<Folder>("folder" + std::to_string(i));
        for (int j = 0; j < 100; ++j) {
            FileType type = static_cast<FileType>(j % 3);
            folder->addChild(std::make_shared<File>("file_" + std::to_string(i) + "_" +
                std::to_string(j), 100, type));
        }
        bigDisk->addChild(folder);
    }

    const int numThreads = 5;
    std::vector<FileCounterVisitor> counters(numThreads);
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&bigDisk, &counters, i]() {
            bigDisk->accept(counters[i]);
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    int textCount = counters[0].getTextFileCount();
    int binaryCount = counters[0].getBinaryFileCount();
    int execCount = counters[0].getExecutableFileCount();

    for (int i = 1; i < numThreads; ++i) {
        EXPECT_EQ(counters[i].getTextFileCount(), textCount);
        EXPECT_EQ(counters[i].getBinaryFileCount(), binaryCount);
        EXPECT_EQ(counters[i].getExecutableFileCount(), execCount);
    }
}

TEST_F(VisitorsTest, VisitorPerformance) {
    auto hugeDisk = std::make_shared<Disk>("HugeDisk", 1000000000);

    for (int i = 0; i < 100; ++i) {
        auto folder = std::make_shared<Folder>("folder" + std::to_string(i));
        for (int j = 0; j < 1000; ++j) {
            folder->addChild(std::make_shared<File>("file" + std::to_string(j),
                1024, FileType::TEXT));
        }
        hugeDisk->addChild(folder);
    }

    auto start = std::chrono::high_resolution_clock::now();

    SizeCalculatorVisitor sizeCalc;
    hugeDisk->accept(sizeCalc);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_LT(duration.count(), 5000);
    EXPECT_EQ(sizeCalc.getTotalSize(), 100 * 1000 * 1024);
}