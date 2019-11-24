#pragma once

#include "types.h"
#include "FileSystem/RamDiskFS.h"
#include "multiboot.h"


/**
 * Note: using the Tar V8 foramt
 */
class TarFS : public RamDiskFS {

public:
    TarFS(multiboot_info_t& mbt) : RamDiskFS(mbt) {}

    File* open(const String& path) override;

};