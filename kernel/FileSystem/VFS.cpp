#include "VFS.h"
#include "path.h"
#include "logging.h"

static VFS* s_the = nullptr;

VFS& VFS::the() {
    if(!s_the) {
        s_the = new VFS();
    }
    return *s_the;
}

void VFS::mount(FileSystem* fs) {
    mounted_filesystems.append(fs);
}

File* VFS::open(const String& path) {
    kprintf("VFS::open %s\n", path.c_str());
    for(auto* fs : mounted_filesystems) {
        ASSERT(fs != nullptr, "VFS::open fs is null");
        if(!path.startswith(fs->mountpoint()))
            continue;
        String inside_path = Path::remove_mount_prefix(path, *fs);
        ASSERT(inside_path != String(""), "VFS::open err in removing FS mount prefix");
        File* f = fs->open(inside_path);
        if(f != nullptr) {
            return f;
        }
    }
    return nullptr;
}