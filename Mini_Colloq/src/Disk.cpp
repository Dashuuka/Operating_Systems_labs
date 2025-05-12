#include "Disk.h"
#include "FileSystemVisitor.h"

Disk::Disk(const std::string& name, size_t totalSize)
    : name(name), totalSize(totalSize) {}

void Disk::accept(FileSystemVisitor& visitor) {
    visitor.visitDisk(*this);
}

std::string Disk::getName() const {
    return name;
}

size_t Disk::getSize() const {
    return totalSize;
}

void Disk::addChild(FileSystemNodePtr child) {
    children.push_back(child);
}

const std::vector<FileSystemNodePtr>& Disk::getChildren() const {
    return children;
}