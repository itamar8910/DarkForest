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

        String to_string() const;

        static IPV4 from_u32(u32 ipv4);
        u32 to_u32();

        static bool from_string(String str, IPV4& out);
    };

    struct MAC
    {
        uint8_t data[MAC_SIZE];

        bool operator==(const MAC& other) const { return !memcmp(data, other.data, sizeof(data)); }
        bool operator!=(const MAC& other) const { return !(*this==other); }

        String to_string() const;
    };

}
