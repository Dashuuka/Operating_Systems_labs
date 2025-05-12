#pragma once

class Disk;
class Folder;
class File;

class FileSystemVisitor {
public:
    virtual ~FileSystemVisitor() = default;
    virtual void visitDisk(Disk& disk) = 0;
    virtual void visitFolder(Folder& folder) = 0;
    virtual void visitFile(File& file) = 0;
};