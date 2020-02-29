#pragma once

#include "device.h"
#include "TaskBlocker.h"
#include "drivers/PS2Mouse.h"

class MouseReadBlocker: public TaskBlocker {
public:
    MouseReadBlocker(): TaskBlocker() {}
    virtual bool can_unblock() override;
    virtual ~MouseReadBlocker() {}
};


class MouseDevice : public Device {
public:
    MouseDevice(const Path& path): Device(path) {}

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;
    int ioctl(u32 request, void* buf) override;

    bool can_read() const override;

private:
    static MouseEvent mouse_read();
};