#pragma once

#include "types.h"

struct ForkArgs
{
    const char* path;
    const char* name;
    char** argv;
    size_t argc;
};
