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

File* VFS::open(const Path& path) {
    kprintf("VFS::open %s\n", path.to_string().c_str());
    for(auto* fs : mounted_filesystems) {
        ASSERT(fs != nullptr);
        kprintf("trying FS: %s\n", fs->mountpoint().to_string().c_str());
        Path inside_path("/");
        bool rc = path.remove_mount_prefix(*fs, inside_path);
        if(!rc) {
            continue;
        }
        File* f = fs->open(inside_path);
        if(f != nullptr) {
            return f;
        }
    }

    return nullptr;
}

bool VFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    (void)path;
    (void)res;
    return false;
}