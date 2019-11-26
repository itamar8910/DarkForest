
#include "FileUtils.h"
#include "types.h"
#include "VFS.h"
#include "errs.h"
#include "logging.h"

u8* FileUtils::read_all(CharFile& f, size_t& size) {
    size = f.size();
    u8* buff = new u8[size];
    int res = f.read(size, buff);
    if(res != (int)size) {
        delete[] buff;
        return nullptr;
    }
    return buff;
}

int FileUtils::file_size(const String& path) {
    kprint("a\n");
    File* f = VFS::the().open(path);
    kprint("b\n");
    if(f == nullptr) {
        return E_NOTFOUND;
    }
    kprint("c\n");
    // TODO: dynamic_cast
    auto* char_f = static_cast<CharFile*>(f);
    kprint("d\n");
    return char_f->size();
    
}