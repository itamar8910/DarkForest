#pragma once
#include "types/String.h"
#include "types.h"

class File {
protected:
    File(const String& path): m_path(path) {}
    virtual ~File(){};

public:
    virtual int read(size_t count, void* buf) = 0;
    virtual int write(char* data, size_t count) = 0;

    const String& path() {return m_path;}

private:
    String m_path;
};

