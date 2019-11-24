
#include "FileUtils.h"
#include "types.h"

u8* FileUtils::read_all(CharFile& f, size_t& size) {
    size = f.size();
    u8* buff = new u8[size];
    int res = f.read(size, buff);
    if(res != (int)size) {
        return nullptr;
    }
    return buff;
}