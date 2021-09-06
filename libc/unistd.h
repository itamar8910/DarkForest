#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void exit(int status);

typedef int off_t;

off_t lseek(int fd, off_t offset, int whence);

u32 df_time_since_boot_ms();

void df_sleep_ms(size_t ms);

#ifdef __cplusplus
}
#endif
