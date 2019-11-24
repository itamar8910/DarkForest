#pragma once

#include "types/String.h"
#include "FileSystem.h"

namespace Path {
    /**
     * e.g (/dev/keyboard, DevFS) => keyboard
     */
String remove_mount_prefix(const String& path, 
                            const FileSystem& fs);
}