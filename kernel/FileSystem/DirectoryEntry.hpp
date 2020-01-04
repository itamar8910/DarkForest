#pragma once

#include "path.h"

struct DirectoryEntry
{
    enum class Type
    {
        File,
        Directory,
    };
    Path path;
    Type type;

    DirectoryEntry(const Path& path, Type type)
        : path(path), type(type) {}
};