
#include "Math.h"
namespace Math
{
    u32 min(u32 x, u32 y)
    {
        return (x < y) ? x : y;
    }

    u32 div_ceil(u32 e, u32 d)
    {
        u32 leftover = e % d;
        u32 floor = e / d;
        return (leftover == 0) ? floor : floor + 1;

    }
    
    u32 round_up(u32 val, u32 m)
    {
        if((val % m) == 0)
            return val;
        return val + (m-(val%m));
    }

    int clamp(int val, int min, int max)
    {
        if(val < min)
        {
            val = min;
        }
        if(val > max)
        {
            val = max;
        }
        return val;
    }
}