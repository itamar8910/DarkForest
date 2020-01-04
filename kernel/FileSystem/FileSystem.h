#pragma once

#include "types/String.h"
#include "file.h"
#include "path.h"

class FileSystem {
public:
    virtual File* open(const Path& path) = 0;
    virtual ~FileSystem(){};

    Path mountpoint() const {return m_mount_point;}

protected:
    FileSystem(Path mount_point): m_mount_point(mount_point) {}


    Path m_mount_point;

};