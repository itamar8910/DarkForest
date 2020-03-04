#pragma once

#include "device.h"
#include "types/vector.h"

class Pipe : public Device {
public:
    Pipe(const Path& path, bool blocking)
        : Device(path), m_blocking(blocking)
        {
        }

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;
    int ioctl(u32 request, void* buf) override;
    bool can_read() const override;

private:

    void block_until_can_read();

    Vector<char> m_data {};
    u32 m_read_idx {0};
    bool m_blocking;
};