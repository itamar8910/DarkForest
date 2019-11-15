#pragma once

#include "types/String.h"
#include "file.h"

class FileSystem {
public:
    virtual File* open(const String& path) = 0;

protected:
    FileSystem(String mount_point): m_mount_point(mount_point) {}


    String m_mount_point;

};