#pragma once

#include "kernel/file.h"
#include "types/vector.h"
#include "shared_ptr.h"

class CharFile : public File {
public:
    CharFile(const String& path, shared_ptr<Vector<u8>> data, size_t size);
    virtual int read(size_t count, void* buf) override;
    virtual int write(char* data, size_t count) override;

    size_t size(){return m_size;}

    // TODO: return a smart pointer
    char* get_content();

private:
    shared_ptr<Vector<u8>> m_data;
    size_t m_size;
    size_t m_idx;
};
