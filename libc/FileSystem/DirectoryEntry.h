#pragma once

#include "path.h"

struct DirectoryEntry
{
    enum class Type : u8
    {
        File,
        Directory,
    };
    Path path;
    Type type;

    DirectoryEntry(const Path& path, Type type)
        : path(path), type(type) {}
    

    // if data is nullptr, just returns length to be serialized
    size_t serialize(u8* data) const;

    // construct DirectoryEntry object out of 'data', returns number of bytes use via out_size
    static DirectoryEntry deserialize(u8* data, size_t& out_size);

};

// DirectoryEntry DirectoryEntry::deserialize(u8* data, size_t& out_size) 