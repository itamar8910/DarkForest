#pragma once
#include "types.h"

struct Point
{
    int x;
    int y;
};

struct Rectangle
{
    int x;
    int y;
    u32 width;
    u32 height;

    bool intersects(const Point& p) const;
};
