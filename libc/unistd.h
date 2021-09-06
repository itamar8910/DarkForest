#pragma once


#ifdef __cplusplus
extern "C" {
#endif

void exit(int status);

typedef int off_t;

off_t lseek(int fd, off_t offset, int whence);


#ifdef __cplusplus
}
#endif
