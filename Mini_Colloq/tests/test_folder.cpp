#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Folder.h"
#include "File.h"
#include "FileSystemVisitor.h"

class FolderTest : public ::testing::Test {
protected:
    void SetUp() override {
        folder = std::make_shared<Folder>("TestFolder");
    }

    std::shared_ptr<Folder> folder;
};

TEST_F(FolderTest, ConstructorInitializes) {
    EXPECT_EQ(folder->getName(), "TestFolder");
    EXPECT_EQ(folder->getSize(), 0);
    EXPECT_TRUE(folder->getChildren().empty());
}

TEST_F(FolderTest, EmptyFolderSize) {
    EXPECT_EQ(folder->getSize(), 0);
}

TEST_F(FolderTest, SizeWithOneFile) {
    auto file = std::make_shared<File>("test.txt", 1024, FileType::TEXT);
    folder->addChild(file);

    EXPECT_EQ(folder->getSize(), 1024);
}

TEST_F(FolderTest, SizeWithMultipleFiles) {
    folder->addChild(std::make_shared<File>("file1.txt", 1000, FileType::TEXT));
    folder->addChild(std::make_shared<File>("file2.bin", 2000, FileType::BINARY));
    folder->addChild(std::make_shared<File>("file3.exe", 3000, FileType::EXECUTABLE));

    EXPECT_EQ(folder->getSize(), 6000);
}

TEST_F(FolderTest, SizeWithNestedFolders) {
    auto subFolder = std::make_shared<Folder>("SubFolder");
    subFolder->addChild(std::make_shared<File>("nested.txt", 500, FileType::TEXT));

    folder->addChild(std::make_shared<File>("file.txt", 1000, FileType::TEXT));
    folder->addChild(subFolder);

    EXPECT_EQ(folder->getSize(), 1500);
}

TEST_F(FolderTest, DeepNesting) {
    auto current = folder;
    const int depth = 100;

    for (int i = 0; i < depth; ++i) {
        auto nextFolder = std::make_shared<Folder>("Folder" + std::to_string(i));
        current->addChild(nextFolder);
        current = nextFolder;
    }

    current->addChild(std::make_shared<File>("deep.txt", 42, FileType::TEXT));

    EXPECT_EQ(folder->getSize(), 42);
}

TEST_F(FolderTest, CircularReferenceHandling) {
    auto folder1 = std::make_shared<Folder>("Folder1");
    auto folder2 = std::make_shared<Folder>("Folder2");

    folder1->addChild(folder2);
    folder2->addChild(folder1);

}

TEST_F(FolderTest, EmptyFolderName) {
    auto emptyNameFolder = std::make_shared<Folder>("");
    EXPECT_EQ(emptyNameFolder->getName(), "");
}

TEST_F(FolderTest, LongFolderName) {
    std::string longName(1000, 'a');
    auto longNameFolder = std::make_shared<Folder>(longName);
    EXPECT_EQ(longNameFolder->getName(), longName);
}

class MockVisitor : public FileSystemVisitor {
public:
    MOCK_METHOD(void, visitDisk, (Disk& disk), (override));
    MOCK_METHOD(void, visitFolder, (Folder& folder), (override));
    MOCK_METHOD(void, visitFile, (File& file), (override));
};

TEST_F(FolderTest, AcceptVisitor) {
    ::testing::StrictMock<MockVisitor> visitor;
    EXPECT_CALL(visitor, visitFolder(::testing::Ref(*folder))).Times(1);

    folder->accept(visitor);
}

TEST_F(FolderTest, GetChildrenReturnsCopy) {
    auto file = std::make_shared<File>("test.txt", 100, FileType::TEXT);
    folder->addChild(file);

    auto children1 = folder->getChildren();
    auto children2 = folder->getChildren();

    EXPECT_EQ(children1.size(), children2.size());
    EXPECT_EQ(children1[0], children2[0]);
}