#include "VFS.h"
#include "libc/FileSystem/path.h"
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
    if(path.num_parts() == 0)
    {
        // this is not really correct because
        // in the future not all filesystem must be mounted in /
        // + there could be things in / which are not filesystems
        for(auto* fs : mounted_filesystems)
        {
           res.append(DirectoryEntry(fs->mountpoint(), DirectoryEntry::Type::Directory));
        }
        return true;
    }
    kprintf("VFS::ls %s\n", path.to_string().c_str());
    Path inside_path("/");
    FileSystem* fs = get_fs(path, inside_path);
    if(fs == nullptr)
        return false;
    return fs->list_directory(inside_path, res);
}

bool VFS::does_directory_exist(const Path& path)
{
    if(path.num_parts() == 0)
    {
        return true;
    }
    
    kprintf("VFS::does_directory_exist %s\n", path.to_string().c_str());
    Path inside_path("/");
    FileSystem* fs = get_fs(path, inside_path);

    if(fs == nullptr)
    {
        return false;
    }

    return fs->does_directory_exist(inside_path);
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


bool VFS::create_file(const Path& path)
{
    (void)path;
    NOT_IMPLEMENTED();
}