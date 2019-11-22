#pragma once

#include "file.h"

class Device: public File {
protected:
    Device(String path): File(path) {}

public:
    virtual int ioctl(u32 request, void* buf) = 0;
    virtual ~Device() {};

};