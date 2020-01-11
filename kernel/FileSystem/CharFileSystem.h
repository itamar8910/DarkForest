#pragma once
#include "FileSystem/FileSystem.h"
#include "FileSystem/path.h"
#include "shared_ptr.h"
#include "FileSystem/CharDirectoryEntry.h"

class CharFileSystem : public FileSystem
{
public:
    CharFileSystem(const Path& mountpoint) : FileSystem(mountpoint)
    {}

    // virtual shared_ptr<Vector<u8>> read_file(CharDirectoryEntry& entry) const = 0;
    virtual bool read_file(CharDirectoryEntry& entry, u8* data) const = 0;
    virtual int write_file(CharDirectoryEntry& entry, const Vector<u8>& data) = 0;

    virtual u32 cluster_size() const = 0;

};