#pragma once
#include "FileSystemNode.h"

enum class FileType {
    TEXT,
    BINARY,
    EXECUTABLE
};

class File : public FileSystemNode {
private:
    std::string name;
    size_t size;
    FileType type;

public:
    File(const std::string& name, size_t size, FileType type);
    void accept(FileSystemVisitor& visitor) override;
    std::string getName() const override;
    size_t getSize() const override;
    FileType getType() const;
};