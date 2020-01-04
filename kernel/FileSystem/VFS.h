#pragma once

#include "file.h"
#include "types/vector.h"
#include "FileSystem.h"

// the Virtual File System
// all file systems exist as mount points of the VFS
constexpr size_t MAX_PATH_LEN = 256;
class VFS : public FileSystem{

public:
    static VFS& the();

    File* open(const Path& path) override;

    void mount(FileSystem* fs);

private:
    VFS() : FileSystem(Path("/")) {}

    Vector<FileSystem*> mounted_filesystems;

};