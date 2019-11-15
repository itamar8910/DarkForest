#include "VFS.h"

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
    // TODO: change logic to use mount points
    // paths inside filesystems should only refer to things 
    // after the mount point, e.g /dev/usb/usb1 -> usb/usb1
    // also, only search FS with a mounting point that is a prefix of 'path'
    for(size_t i = 0; i < mounted_filesystems.size(); i++) {
        auto* fs = mounted_filesystems[i];
        File* f = fs->open(path);
        if(f != nullptr) {
            return f;
        }
    }
    return nullptr;
}