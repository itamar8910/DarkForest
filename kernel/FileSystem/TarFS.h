#pragma once

#include "types.h"

#include "FileSystem/RamDiskFS.h"


/**
 * Note: using the Tar V8 foramt
 */
class TarFS : public RamDiskFS {

public:
    TarFS(void* base, u32 size) : RamDiskFS(base, size) {}

    u8* get_content(const char* path, u32& size) override;

};