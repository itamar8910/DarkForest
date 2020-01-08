#include "DirectoryEntry.h"

size_t DirectoryEntry::serialize(u8* data) const{
    const String path_str = path.to_string();
    const size_t serialized_size = sizeof(u8) + sizeof(u16) + path_str.len();
    if(data == nullptr)
    {
        return serialized_size;
    }
    size_t offset = 0;
    *(u8*)data = static_cast<u8>(type);
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
    kprintf("deser type: %d\n", static_cast<int>(type));
    out_size+=sizeof(u8);
    u16 path_len = *reinterpret_cast<u16*>(data+out_size);
    kprintf("deser path len: %d\n", path_len);
    out_size += sizeof(u16);
    kprintf("path size: %d\n", path_len);
    String path_str(reinterpret_cast<char*>(data+out_size), path_len);
    out_size += path_len;
    kprintf("deser path str: %s\n", path_str.c_str());
    return DirectoryEntry(Path(path_str), type);
}