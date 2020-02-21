#pragma once

#include "device.h"
#include "TaskBlocker.h"
#include "drivers/PS2Keyboard.h"

class KeyboardReadBlocker: public TaskBlocker {
public:
    KeyboardReadBlocker(): TaskBlocker() {}
    virtual bool can_unblock() override;
    virtual ~KeyboardReadBlocker() {}
};

KeyEvent keyboard_read();


class KeyboardDevice : public Device {
public:
    KeyboardDevice(const Path& path): Device(path) {}

    int read(size_t count, void* buf) override;
    int write(char* data, size_t count) override;
    int ioctl(u32 request, void* buf) override;

    bool can_read() const override;
};