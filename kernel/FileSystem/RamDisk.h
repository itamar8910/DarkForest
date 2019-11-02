#pragma once
#include "types.h"
#include "FileSystem/RamDiskFS.h"

namespace RamDisk {
    void init(void* base, u32 size);
    RamDiskFS& fs();
};