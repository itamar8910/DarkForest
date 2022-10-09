#pragma once

#include "types.h"
#include "constants.h"

namespace Network
{
    static inline uint16_t internet_checksum(const u8* data, size_t size)
    {
        uint32_t sum = 0;

        for (size_t index = 0; index < size; index += 2)
        {
            uint16_t current = 0;
            if (index < size - 1)
            {
                current = *reinterpret_cast<const uint16_t*>(data + index);
            } else {
                current = data[index]<<8;
            }
            sum += current;
            if (sum > 0xFFFF)
            {
                sum -= 0XFFFF;
            }

        }
        uint16_t sum_16bit = (uint16_t) sum;
        return ~sum_16bit;
    }
}