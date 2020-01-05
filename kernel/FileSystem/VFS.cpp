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
    Path inside_path("/");
    FileSystem* fs = get_fs(path, inside_path);
    if(fs == nullptr)
        return nullptr;

    return fs->open(inside_path);
}

bool VFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    kprintf("VFS::ls %s\n", path.to_string().c_str());
    Path inside_path("/");
    FileSystem* fs = get_fs(path, inside_path);
    if(fs == nullptr)
        return false;
    return fs->list_directory(inside_path, res);
}

FileSystem* VFS::get_fs(const Path& path, Path& path_inside_fs)
{
    for(auto* fs : mounted_filesystems) {
        ASSERT(fs != nullptr);
        Path inside_path("/");
        bool rc = path.remove_mount_prefix(*fs, inside_path);
        if(!rc) {
            continue;
        }
        path_inside_fs = inside_path;
        return fs;
    }
    return nullptr;
}