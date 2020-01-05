#pragma once

#include "types/String.h"
#include "file.h"
#include "path.h"
#include "DirectoryEntry.hpp"

class FileSystem {
public:
    virtual ~FileSystem(){};

    virtual File* open(const Path& path) = 0;

    virtual bool list_directory(const Path& path, Vector<DirectoryEntry>& res) = 0;

    Path mountpoint() const {return m_mount_point;}

protected:
    FileSystem(Path mount_point): m_mount_point(mount_point) {}


    Path m_mount_point;

};