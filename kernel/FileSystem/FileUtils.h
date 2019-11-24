#pragma once

#include "kernel/FileSystem/CharFile.h"
#include "types.h"

namespace FileUtils {
    u8* read_all(CharFile& f, size_t& size);
}