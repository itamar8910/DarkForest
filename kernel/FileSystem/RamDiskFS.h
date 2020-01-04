#pragma once
#include "types.h"
#include "FileSystem.h"
#include "multiboot.h"

class RamDiskFS : public FileSystem{
public:
    virtual File* open(const Path& path) = 0;

    void* base(){return m_base;}
    u32 size(){return m_size;}

    // ramdiskFS does not need to free the memory (its from bootloader)
    virtual ~RamDiskFS(){};

protected:
    RamDiskFS(multiboot_info_t& mbt);

private:
    void* m_base {nullptr};
    u32 m_size {0};
};