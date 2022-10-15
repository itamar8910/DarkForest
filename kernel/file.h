#pragma once
#include "FileSystem/path.h"
#include "types.h"

class File {
protected:
    File(const Path& path): m_path(path) {}

public:
    virtual ~File(){};

    virtual int read(size_t count, void* buf) = 0;
    virtual int write(char* data, size_t count) = 0;
    virtual int lseek(int offset, int whence) {(void)offset; (void)whence; return  0;}

    virtual bool can_read() const {return false;}

    const Path& path() {return m_path;}


private:
    Path m_path;
};
