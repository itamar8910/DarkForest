#pragma once
#include "types.h"

class RamDiskFS {
public:
    /*
     * Read content of file
     * 
     * path - path of file
     * size - the size of the content would be returned 
     *        via this parameter
     * Returns pointer to data, or nullptr if path not found
     */
    virtual u8* get_content(const char* path, u32& size) = 0;
protected:
    RamDiskFS(void* base, u32 size)
        : m_base(base),
          m_size(size) {}

private:
    void* m_base;
    u32 m_size;
};