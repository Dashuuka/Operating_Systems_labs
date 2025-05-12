#include "File.h"
#include "FileSystemVisitor.h"

File::File(const std::string& name, size_t size, FileType type)
    : name(name), size(size), type(type) {}

void File::accept(FileSystemVisitor& visitor) {
    visitor.visitFile(*this);
}

std::string File::getName() const {
    return name;
}

size_t File::getSize() const {
    return size;
}

FileType File::getType() const {
    return type;
}