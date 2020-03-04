#pragma once

#include "device.h"

class VgaDevice : public Device {
public:
    VgaDevice(const Path& path);

    int read(size_t count, void* buf) override {(void)count;(void)buf;return E_NOT_SUPPORTED;}
    int write(char* data, size_t count) override {(void)data;(void)count;return E_NOT_SUPPORTED;}
    int ioctl(u32 request, void* buf) override;
    int mmap(void* virtual_addr, u32 size) override;

private:
    void initialize();

private:
    u16 m_width;
    u16 m_height;
    u16 m_pitch;
};