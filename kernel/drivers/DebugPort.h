#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Cstring.h"

#define DEBUG_PORT_ADDR 0xe9

class DebugPort {

    private:
        static void write(const char*, size_t size);
    public:
        static void write(const char*);

};