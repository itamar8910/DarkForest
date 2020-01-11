#pragma once

#include "path.h"

class DirectoryEntry
{
public:
    enum class Type : u8
    {
        File,
        Directory,
    };

    DirectoryEntry(const Path& path, Type type)
        : m_path(path), m_type(type) {}
    

    // if data is nullptr, just returns length to be serialized
    size_t serialize(u8* data) const;

    // construct DirectoryEntry object out of 'data', returns number of bytes use via out_size
    static DirectoryEntry deserialize(u8* data, size_t& out_size);

    Path path() const{return m_path;}
    Type type() const{return m_type;}

private:
    Path m_path;
    Type m_type;
};
