#include "DirectoryEntry.h"

size_t DirectoryEntry::serialize(u8* data) const{
    const String path_str = m_path.to_string();
    const size_t serialized_size = sizeof(u8) + sizeof(u16) + path_str.len();
    if(data == nullptr)
    {
        return serialized_size;
    }
    size_t offset = 0;
    *(u8*)data = static_cast<u8>(m_type);
    offset+=sizeof(u8);
    *(u16*)(data+offset) = static_cast<u16>(path_str.len());
    offset+=sizeof(u16);
    memcpy(data+offset, path_str.c_str(), path_str.len());
    offset += path_str.len();
    ASSERT(offset==serialized_size);
    return serialized_size;
}

DirectoryEntry DirectoryEntry::deserialize(u8* data, size_t& out_size)
{ 
    out_size = 0;
    Type type = static_cast<Type>(*(data+out_size));
    out_size+=sizeof(u8);
    u16 path_len = *reinterpret_cast<u16*>(data+out_size);
    out_size += sizeof(u16);
    String path_str(reinterpret_cast<char*>(data+out_size), path_len);
    out_size += path_len;
    return DirectoryEntry(Path(path_str), type);
}