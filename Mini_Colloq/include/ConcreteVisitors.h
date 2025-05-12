#pragma once
#include "FileSystemVisitor.h"
#include <iostream>
#include <atomic>

class PrintVisitor : public FileSystemVisitor {
private:
    int indentLevel = 0;
    void printIndent() const;

public:
    void visitDisk(Disk& disk) override;
    void visitFolder(Folder& folder) override;
    void visitFile(File& file) override;
};

class SizeCalculatorVisitor : public FileSystemVisitor {
private:
    std::atomic<size_t> totalSize{ 0 };

public:
    void visitDisk(Disk& disk) override;
    void visitFolder(Folder& folder) override;
    void visitFile(File& file) override;
    size_t getTotalSize() const;
};

class FileCounterVisitor : public FileSystemVisitor {
private:
    std::atomic<int> textFiles{ 0 };
    std::atomic<int> binaryFiles{ 0 };
    std::atomic<int> executableFiles{ 0 };

public:
    void visitDisk(Disk& disk) override;
    void visitFolder(Folder& folder) override;
    void visitFile(File& file) override;

    int getTextFileCount() const { return textFiles; }
    int getBinaryFileCount() const { return binaryFiles; }
    int getExecutableFileCount() const { return executableFiles; }
};