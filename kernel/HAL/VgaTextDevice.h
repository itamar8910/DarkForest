
#pragma once

#include "device.h"
#include "drivers/VgaText.h"

class VgaTextDevice : public Device {
public:
    VgaTextDevice(const Path& path)
        : Device(path)
            {
            }

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;
    int ioctl(u32 request, void* buf) override;
};