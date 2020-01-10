#pragma once

#include "kernel/file.h"
#include "types/vector.h"
#include "shared_ptr.h"
#include "FileSystem/DirectoryEntry.h"
#include "FileSystem/CharFileSystem.h"

class CharFile : public File {
public:
    CharFile(const Path& path, CharFileSystem& fs, CharDirectoryEntry dir_entry, size_t size)
        : File(path),
          m_fs(fs),
          m_dir_entry(dir_entry),
          m_size(size),
          m_idx(0) 
          {}

    virtual int read(size_t count, void* buf) override;
    virtual int write(char* data, size_t count) override;

    size_t size(){return m_size;}

    // TODO: return a smart pointer
    char* get_content();

private:
    CharFileSystem& m_fs;
    CharDirectoryEntry m_dir_entry;
    size_t m_size;
    size_t m_idx;
};
