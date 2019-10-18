#pragma once

#include "new.h"
// miscellaneous std functions

template<typename T>
T&& move(T& t) {
    return static_cast<T&&>(t);
}