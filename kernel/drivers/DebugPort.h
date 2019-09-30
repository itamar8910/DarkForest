#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Cstring.h"

#define DEBUG_PORT_ADDR 0xe9

namespace DebugPort {
    void write(const char*, size_t size);
    void write(const char*);
    void write_dec(int);
};