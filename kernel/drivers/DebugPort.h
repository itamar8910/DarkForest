#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "string.h"

#define DEBUG_PORT_ADDR 0xe9

namespace DebugPort {
    void putc(char c);
    void write(const char*, size_t size);
    void write(const char*);
};