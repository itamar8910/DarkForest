#pragma once

#include "file.h"
#include "types/vector.h"
#include "FileSystem.h"

// the Virtual File System
// all file systems exist as mount points of the VFS
class VFS : public FileSystem{

public:
    static VFS& the();

    File* open(const String& path) override;

    void mount(FileSystem* fs);

private:
    VFS() : FileSystem("/") {}

    Vector<FileSystem*> mounted_filesystems;

};