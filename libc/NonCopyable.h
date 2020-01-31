#pragma once

#define NON_COPYABLE(name) \
public: \
    name(name& other) = delete; \
    name& operator=(name& other) = delete; \

