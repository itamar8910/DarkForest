#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


int atoi(const char* str);
int df_atoi(const char* str, size_t base);
int abs(int j);
int itoa(int num, char* out, u32 out_len);
double atof(const char *nptr);


#ifdef __cplusplus
}
#endif
