#pragma once
#include "FileSystemNode.h"
#include <vector>

class Folder : public FileSystemNode {
private:
    std::string name;
    std::vector<FileSystemNodePtr> children;

public:
    explicit Folder(const std::string& name);
    void accept(FileSystemVisitor& visitor) override;
    std::string getName() const override;
    size_t getSize() const override;
    void addChild(FileSystemNodePtr child);
    const std::vector<FileSystemNodePtr>& getChildren() const;
};