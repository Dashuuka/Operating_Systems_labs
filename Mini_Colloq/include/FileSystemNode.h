#pragma once
#include <vector>
#include <string>
#include <memory>

class FileSystemVisitor;

class FileSystemNode {
public:
    virtual ~FileSystemNode() = default;
    virtual void accept(FileSystemVisitor& visitor) = 0;
    virtual std::string getName() const = 0;
    virtual size_t getSize() const = 0;
};

using FileSystemNodePtr = std::shared_ptr<FileSystemNode>;