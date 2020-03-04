#pragma once

#include "file.h"
#include "errs.h"

class Device: public File {
protected:
    Device(const Path& path): File(path) {}

public:
    virtual int ioctl(u32 request, void* buf) = 0;
    virtual int mmap(void* virtual_addr, u32 size) {(void)virtual_addr;(void)size;return E_NOT_SUPPORTED;}
    virtual ~Device() {};

};