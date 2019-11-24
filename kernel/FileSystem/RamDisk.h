#pragma once

#include "types.h"
#include "FileSystem/RamDiskFS.h"
#include "multiboot.h"

namespace RamDisk {
    void init(multiboot_info_t& mbt);
    RamDiskFS& fs();
};