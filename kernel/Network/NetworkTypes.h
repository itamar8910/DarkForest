#pragma once

#include "types.h"
#include "cstring.h"
#include "types/String.h"

namespace Network
{

    constexpr size_t MAC_SIZE = 6;
    constexpr size_t IPV4_SIZE = 4;

    struct IPV4
    {
        uint8_t data[IPV4_SIZE];

        bool operator==(const IPV4& other) const { return !memcmp(data, other.data, sizeof(data)); }
        bool operator!=(const IPV4& other) const { return !(*this==other); }
    };

    struct MAC
    {
        uint8_t data[MAC_SIZE];

        bool operator==(const MAC& other) const { return !memcmp(data, other.data, sizeof(data)); }
        bool operator!=(const MAC& other) const { return !(*this==other); }

        String to_string() const;
    };

}
