#pragma once

#include "types.h"

struct __attribute__((__packed__)) IcmpEchoHeader
{
    enum class Type : uint8_t
    {
        Request=8,
        Reply=0, 
    };

    Type type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
};