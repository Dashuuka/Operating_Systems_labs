#pragma once
#include "FileSystemNode.h"
#include <vector>

class Disk : public FileSystemNode {
private:
    std::string name;
    std::vector<FileSystemNodePtr> children;
    size_t totalSize;

public:
    Disk(const std::string& name, size_t totalSize);
    void accept(FileSystemVisitor& visitor) override;
    std::string getName() const override;
    size_t getSize() const override;
    void addChild(FileSystemNodePtr child);
    const std::vector<FileSystemNodePtr>& getChildren() const;
};