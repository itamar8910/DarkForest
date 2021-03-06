#pragma once

#include "types/String.h"
#include "kernel/file.h"
#include "libc/FileSystem/path.h"
#include "libc/FileSystem/DirectoryEntry.h"

class FileSystem {
public:
    virtual ~FileSystem(){};

    virtual File* open(const Path& path) = 0;

    virtual bool list_directory(const Path& path, Vector<DirectoryEntry>& res) = 0;
    
    virtual bool is_directory(const Path& path) = 0;
    virtual bool is_file(const Path& path) = 0;

    Path mountpoint() const {return m_mount_point;}

    virtual bool create_entry(const Path& path, DirectoryEntry::Type type) = 0;

protected:
    FileSystem(Path mount_point): m_mount_point(mount_point) {}


    Path m_mount_point;

};
