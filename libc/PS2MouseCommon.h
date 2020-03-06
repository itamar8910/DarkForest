#pragma once
#include "types.h"

struct RawMouseEvent
{
    int32_t delta_x;
    int32_t delta_y;
    bool left_button;
    bool right_button;
};