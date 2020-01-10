#pragma once
#include "FileSystem/DirectoryEntry.h"

class CharDirectoryEntry : public DirectoryEntry
{

public:

    CharDirectoryEntry(const Path& path, Type type, u32 file_size, u32 cluster_idx)
        : DirectoryEntry(path, type),
          m_file_size(file_size),
          m_cluster_idx(cluster_idx)
          {}

    u32 file_size() const{return m_file_size;}
    u32 cluster_idx() const{return m_cluster_idx;}

private:
    u32 m_file_size;
    u32 m_cluster_idx;
};