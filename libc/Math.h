#pragma once
#include "types.h"
namespace Math
{
    u32 min(u32 x, u32 y);
    u32 div_ceil(u32 e, u32 d);
    
    // return val rounded up to be a multiple of m
    u32 round_up(u32 val, u32 m);
}