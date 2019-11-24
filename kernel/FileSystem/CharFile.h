#pragma once

#include "kernel/file.h"

class CharFile : public File {
public:
    CharFile(const String& path, u8* data, size_t size);
    virtual int read(size_t count, void* buf) override;
    virtual int write(char* data, size_t count) override;

    size_t size(){return m_size;}

private:
    u8* m_data;
    size_t m_size;
    size_t m_idx;
};
