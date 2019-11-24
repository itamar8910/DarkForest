#include "FileSystem/RamDisk.h"
#include "FileSystem/TarFS.h"
#include "multiboot.h"

static RamDiskFS* s_the = nullptr;

void RamDisk::init(multiboot_info_t& mbt) {
    s_the = new TarFS(mbt);
}

RamDiskFS& RamDisk::fs() {
    return *s_the;
}