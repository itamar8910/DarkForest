
#include "FileUtils.h"
#include "types.h"
#include "VFS.h"
#include "errs.h"
#include "logging.h"

u8* FileUtils::read_all(CharFile& f, size_t& size) {
    // TODO: return a shared_ptr
    size = f.size();
    u8* buff = new u8[size];
    int res = f.read(size, buff);
    kprintf("res: %d, size: %d\n", res, size);
    if(res != (int)size) {
        delete[] buff;
        return nullptr;
    }
    return buff;
}

int FileUtils::file_size(const String& path) {
    File* f = VFS::the().open(Path(path));
    if(f == nullptr) {
        return E_NOTFOUND;
    }
    // TODO: dynamic_cast
    auto* char_f = static_cast<CharFile*>(f);
    return char_f->size();
    
}