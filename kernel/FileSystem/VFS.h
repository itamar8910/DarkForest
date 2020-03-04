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

    virtual bool list_directory(const Path& path, Vector<DirectoryEntry>& res) override;

    virtual bool is_directory(const Path& path) override;
    virtual bool is_file(const Path& path) override;

    void mount(FileSystem* fs);

    virtual bool create_entry(const Path& path, DirectoryEntry::Type type) override;

private:
    VFS() : FileSystem(Path("/")) {}

    FileSystem* get_fs(const Path& path, Path& path_inside_fs);

    Vector<FileSystem*> mounted_filesystems;

};
