#pragma once
#include "types/String.h"
#include "types.h"
class File {
protected:
    File(String path): m_path(path) {}

public:
    virtual int read(size_t count, void* buf) = 0;
    virtual int write(char* data, size_t count) = 0;

private:
    String m_path;
};

