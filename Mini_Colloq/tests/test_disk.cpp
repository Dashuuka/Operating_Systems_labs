#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include "FileSystemVisitor.h"
#include <limits>

using ::testing::_;
using ::testing::StrictMock;

class MockVisitor : public FileSystemVisitor {
public:
    MOCK_METHOD(void, visitDisk, (Disk& disk), (override));
    MOCK_METHOD(void, visitFolder, (Folder& folder), (override));
    MOCK_METHOD(void, visitFile, (File& file), (override));
};

class DiskTest : public ::testing::Test {
protected:
    void SetUp() override {
        disk = std::make_shared<Disk>("TestDisk", 1000000);
    }

    std::shared_ptr<Disk> disk;
};

TEST_F(DiskTest, ConstructorInitializes) {
    EXPECT_EQ(disk->getName(), "TestDisk");
    EXPECT_EQ(disk->getSize(), 1000000);
    EXPECT_TRUE(disk->getChildren().empty());
}

TEST_F(DiskTest, EmptyDiskName) {
    auto emptyDisk = std::make_shared<Disk>("", 1000);
    EXPECT_EQ(emptyDisk->getName(), "");
}

TEST_F(DiskTest, ZeroSize) {
    auto zeroDisk = std::make_shared<Disk>("Zero", 0);
    EXPECT_EQ(zeroDisk->getSize(), 0);
}

TEST_F(DiskTest, MaximumSize) {
    auto maxDisk = std::make_shared<Disk>("Max", std::numeric_limits<size_t>::max());
    EXPECT_EQ(maxDisk->getSize(), std::numeric_limits<size_t>::max());
}

TEST_F(DiskTest, AddSingleChild) {
    auto file = std::make_shared<File>("test.txt", 100, FileType::TEXT);
    disk->addChild(file);

    EXPECT_EQ(disk->getChildren().size(), 1);
    EXPECT_EQ(disk->getChildren()[0], file);
}

TEST_F(DiskTest, AddMultipleChildren) {
    auto file1 = std::make_shared<File>("file1.txt", 100, FileType::TEXT);
    auto file2 = std::make_shared<File>("file2.txt", 200, FileType::BINARY);
    auto folder = std::make_shared<Folder>("folder");

    disk->addChild(file1);
    disk->addChild(file2);
    disk->addChild(folder);

    EXPECT_EQ(disk->getChildren().size(), 3);
    EXPECT_EQ(disk->getChildren()[0], file1);
    EXPECT_EQ(disk->getChildren()[1], file2);
    EXPECT_EQ(disk->getChildren()[2], folder);
}

TEST_F(DiskTest, AddNullChild) {
    disk->addChild(nullptr);
    EXPECT_EQ(disk->getChildren().size(), 1);
    EXPECT_EQ(disk->getChildren()[0], nullptr);
}

TEST_F(DiskTest, AcceptVisitor) {
    StrictMock<MockVisitor> visitor;
    EXPECT_CALL(visitor, visitDisk(::testing::Ref(*disk))).Times(1);

    disk->accept(visitor);
}

TEST_F(DiskTest, GetChildrenConst) {
    const auto constDisk = disk;
    const auto& children = constDisk->getChildren();
    EXPECT_TRUE(children.empty());
}

TEST_F(DiskTest, SpecialCharactersInName) {
    auto specialDisk = std::make_shared<Disk>("C:\\Program Files\\Test", 5000);
    EXPECT_EQ(specialDisk->getName(), "C:\\Program Files\\Test");
}

TEST_F(DiskTest, UnicodeCharactersInName) {
    auto unicodeDisk = std::make_shared<Disk>("Disk_Test_123", 1000);
    EXPECT_EQ(unicodeDisk->getName(), "Disk_Test_123");
}