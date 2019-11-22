
#pragma once

#include "device.h"
#include "drivers/VgaText.h"

class VgaTextDevice : public Device {
public:

    enum class IOCTL_CODE : u32{
        PUT_CHAR = 1,
        MOVE_CURSOR = 2,
        GET_CHAR = 3,
        CLEAR = 4,
    };

    struct IOCTL_DATA {
        u8 row {0};
        u8 col {0};
        u16 value {0};
    };

    enum class IOCTL_ERRS {
        E_OUT_OF_BOUNDS = 1,
    };

    VgaTextDevice(String path)
        : Device(path)
            {
            }

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;
    int ioctl(u32 request, void* buf) override;

};