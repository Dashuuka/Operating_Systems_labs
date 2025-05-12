#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "File.h"
#include "FileSystemVisitor.h"

class FileTest : public ::testing::Test {
protected:
    void SetUp() override {
        textFile = std::make_shared<File>("test.txt", 1024, FileType::TEXT);
        binaryFile = std::make_shared<File>("test.bin", 2048, FileType::BINARY);
        execFile = std::make_shared<File>("test.exe", 4096, FileType::EXECUTABLE);
    }

    std::shared_ptr<File> textFile;
    std::shared_ptr<File> binaryFile;
    std::shared_ptr<File> execFile;
};

TEST_F(FileTest, ConstructorInitializesText) {
    EXPECT_EQ(textFile->getName(), "test.txt");
    EXPECT_EQ(textFile->getSize(), 1024);
    EXPECT_EQ(textFile->getType(), FileType::TEXT);
}

TEST_F(FileTest, ConstructorInitializesBinary) {
    EXPECT_EQ(binaryFile->getName(), "test.bin");
    EXPECT_EQ(binaryFile->getSize(), 2048);
    EXPECT_EQ(binaryFile->getType(), FileType::BINARY);
}

TEST_F(FileTest, ConstructorInitializesExecutable) {
    EXPECT_EQ(execFile->getName(), "test.exe");
    EXPECT_EQ(execFile->getSize(), 4096);
    EXPECT_EQ(execFile->getType(), FileType::EXECUTABLE);
}

TEST_F(FileTest, ZeroSizeFile) {
    auto zeroFile = std::make_shared<File>("empty.txt", 0, FileType::TEXT);
    EXPECT_EQ(zeroFile->getSize(), 0);
}

TEST_F(FileTest, MaxSizeFile) {
    auto maxFile = std::make_shared<File>("huge.bin",
        std::numeric_limits<size_t>::max(), FileType::BINARY);
    EXPECT_EQ(maxFile->getSize(), std::numeric_limits<size_t>::max());
}

TEST_F(FileTest, EmptyFileName) {
    auto emptyNameFile = std::make_shared<File>("", 100, FileType::TEXT);
    EXPECT_EQ(emptyNameFile->getName(), "");
}

TEST_F(FileTest, SpecialCharactersInName) {
    auto specialFile = std::make_shared<File>("!@#$%^&*().txt", 100, FileType::TEXT);
    EXPECT_EQ(specialFile->getName(), "!@#$%^&*().txt");
}

TEST_F(FileTest, FileWithPath) {
    auto pathFile = std::make_shared<File>("/home/user/docs/file.txt", 500, FileType::TEXT);
    EXPECT_EQ(pathFile->getName(), "/home/user/docs/file.txt");
}

class MockVisitor : public FileSystemVisitor {
public:
    MOCK_METHOD(void, visitDisk, (Disk& disk), (override));
    MOCK_METHOD(void, visitFolder, (Folder& folder), (override));
    MOCK_METHOD(void, visitFile, (File& file), (override));
};

TEST_F(FileTest, AcceptVisitorText) {
    ::testing::StrictMock<MockVisitor> visitor;
    EXPECT_CALL(visitor, visitFile(::testing::Ref(*textFile))).Times(1);

    textFile->accept(visitor);
}

TEST_F(FileTest, AcceptVisitorBinary) {
    ::testing::StrictMock<MockVisitor> visitor;
    EXPECT_CALL(visitor, visitFile(::testing::Ref(*binaryFile))).Times(1);

    binaryFile->accept(visitor);
}

TEST_F(FileTest, AcceptVisitorExecutable) {
    ::testing::StrictMock<MockVisitor> visitor;
    EXPECT_CALL(visitor, visitFile(::testing::Ref(*execFile))).Times(1);

    execFile->accept(visitor);
}

class FileTypeTest : public ::testing::TestWithParam<FileType> {};

TEST_P(FileTypeTest, FileTypeCorrectlySet) {
    FileType type = GetParam();
    auto file = std::make_shared<File>("test", 100, type);
    EXPECT_EQ(file->getType(), type);
}

INSTANTIATE_TEST_SUITE_P(AllFileTypes, FileTypeTest,
    ::testing::Values(FileType::TEXT, FileType::BINARY, FileType::EXECUTABLE));