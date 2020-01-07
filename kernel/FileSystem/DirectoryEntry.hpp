#pragma once

#include "path.h"

struct DirectoryEntry
{
    enum class Type
    {
        File,
        Directory,
    };
    Path path;
    Type type;

    DirectoryEntry(const Path& path, Type type)
        : path(path), type(type) {}
    

    // TODO: implement
    // if data is nullptr, just returns length to be serialized
    size_t serialize(u8* data) const{
        (void)data;
        NOT_IMPLEMENTED();
        return 0;
    }
    // construct DirectoryEntry object out of 'data', returns number of bytes use via out_size
    static DirectoryEntry deserialize(u8* data, size_t& out_size)
    { 
        (void)data;
        (void)out_size;
        NOT_IMPLEMENTED();
        return DirectoryEntry(Path("/"), Type::Directory);
    }
};

// DirectoryEntry DirectoryEntry::deserialize(u8* data, size_t& out_size) 