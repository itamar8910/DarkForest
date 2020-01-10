#pragma once
#include "types.h"
namespace Math
{
    static u32 min(u32 x, u32 y)
    {
        return (x < y) ? x : y;
    }
    static u32 div_ceil(u32 e, u32 d)
    {
        u32 leftover = e % d;
        u32 floor = e / d;
        return (leftover == 0) ? floor : floor + 1;

    }
}