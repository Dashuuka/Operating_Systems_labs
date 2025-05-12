#include "Folder.h"
#include "FileSystemVisitor.h"

Folder::Folder(const std::string& name) : name(name) {}

void Folder::accept(FileSystemVisitor& visitor) {
    visitor.visitFolder(*this);
}

std::string Folder::getName() const {
    return name;
}

size_t Folder::getSize() const {
    size_t totalSize = 0;
    for (const auto& child : children) {
        if (child) {
            totalSize += child->getSize();
        }
    }
    return totalSize;
}

void Folder::addChild(FileSystemNodePtr child) {
    children.push_back(child);
}

const std::vector<FileSystemNodePtr>& Folder::getChildren() const {
    return children;
}