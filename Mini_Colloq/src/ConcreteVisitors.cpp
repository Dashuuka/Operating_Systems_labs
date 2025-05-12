#include "ConcreteVisitors.h"
#include "Disk.h"
#include "Folder.h"
#include "File.h"
#include <thread>
#include <future>

void PrintVisitor::printIndent() const {
    for (int i = 0; i < indentLevel; ++i) {
        std::cout << "  ";
    }
}

void PrintVisitor::visitDisk(Disk& disk) {
    printIndent();
    std::cout << "[DISK] " << disk.getName() << " (Total: " << disk.getSize() << " bytes)\n";
    indentLevel++;
    for (auto& child : disk.getChildren()) {
        if (child) {
            child->accept(*this);
        }
    }
    indentLevel--;
}

void PrintVisitor::visitFolder(Folder& folder) {
    printIndent();
    std::cout << "[FOLDER] " << folder.getName() << "\n";
    indentLevel++;
    for (auto& child : folder.getChildren()) {
        if (child) {
            child->accept(*this);
        }
    }
    indentLevel--;
}

void PrintVisitor::visitFile(File& file) {
    printIndent();
    std::string typeStr;
    switch (file.getType()) {
    case FileType::TEXT: typeStr = "TEXT"; break;
    case FileType::BINARY: typeStr = "BINARY"; break;
    case FileType::EXECUTABLE: typeStr = "EXEC"; break;
    }
    std::cout << "[FILE] " << file.getName() << " (" << typeStr << ", "
        << file.getSize() << " bytes)\n";
}

void SizeCalculatorVisitor::visitDisk(Disk& disk) {
    for (auto& child : disk.getChildren()) {
        if (child) {
            child->accept(*this);
        }
    }
}

void SizeCalculatorVisitor::visitFolder(Folder& folder) {
    for (auto& child : folder.getChildren()) {
        if (child) {
            child->accept(*this);
        }
    }
}

void SizeCalculatorVisitor::visitFile(File& file) {
    totalSize += file.getSize();
}

size_t SizeCalculatorVisitor::getTotalSize() const {
    return totalSize;
}

void FileCounterVisitor::visitDisk(Disk& disk) {
    std::vector<std::future<void>> futures;
    for (auto& child : disk.getChildren()) {
        if (child) {
            futures.push_back(std::async(std::launch::async, [&child, this]() {
                child->accept(*this);
                }));
        }
    }
    for (auto& future : futures) {
        future.wait();
    }
}

void FileCounterVisitor::visitFolder(Folder& folder) {
    std::vector<std::future<void>> futures;
    for (auto& child : folder.getChildren()) {
        if (child) {
            futures.push_back(std::async(std::launch::async, [&child, this]() {
                child->accept(*this);
                }));
        }
    }
    for (auto& future : futures) {
        future.wait();
    }
}

void FileCounterVisitor::visitFile(File& file) {
    switch (file.getType()) {
    case FileType::TEXT:
        textFiles++;
        break;
    case FileType::BINARY:
        binaryFiles++;
        break;
    case FileType::EXECUTABLE:
        executableFiles++;
        break;
    }
}