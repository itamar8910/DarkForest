#pragma once

#include "types.h"

namespace Network
{

    constexpr size_t MAC_SIZE = 6;
    constexpr size_t IPV4_SIZE = 4;

    struct IPV4
    {
        uint8_t data[IPV4_SIZE];
    };

    struct MAC
    {
        uint8_t data[MAC_SIZE];
    };

}
