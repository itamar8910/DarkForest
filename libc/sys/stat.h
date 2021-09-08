#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t mode_t;
int mkdir(const char *pathname, mode_t mode);


#ifdef __cplusplus
}
#endif


