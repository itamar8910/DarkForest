#pragma once
#include "FileSystem/path.h"
#include "types.h"

class File {
protected:
    File(const Path& path): m_path(path) {}
    virtual ~File(){};

public:
    virtual int read(size_t count, void* buf) = 0;
    virtual int write(char* data, size_t count) = 0;

    const Path& path() {return m_path;}

private:
    Path m_path;
};

