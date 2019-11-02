#include "FileSystem/RamDisk.h"
#include "FileSystem/TarFS.h"

static RamDiskFS* s_the = nullptr;

void RamDisk::init(void* base, u32 size) {
    s_the = new TarFS(base, size);
}

RamDiskFS& fs() {
    return *s_the;
}