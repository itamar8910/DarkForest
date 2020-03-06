#pragma once
#include "types.h"

struct MouseEvent
{
    int32_t delta_x;
    int32_t delta_y;
    bool left_button;
    bool right_button;
};