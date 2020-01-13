#pragma once

#include "kernel/FileSystem/CharFile.h"
#include "types.h"
#include "shared_ptr.h"
#include "BigBuffer.h"

namespace FileUtils {
    shared_ptr<Vector<u8>> read_all(CharFile& f, size_t& size);
    shared_ptr<BigBuffer> read_all_big(CharFile& f, size_t& size);
    int file_size(const String& path);
}