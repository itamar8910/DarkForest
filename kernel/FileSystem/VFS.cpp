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
    {
        kprintf("dint't find matching fs!\n");
        return nullptr;
    }

    return fs->open(inside_path);
}

bool VFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    kprintf("VFS::ls %s\n", path.to_string().c_str());

    for(auto* fs : mounted_filesystems)
    {
        if(path.is_parent_of(fs->mountpoint()))
        {
            res.append(DirectoryEntry(fs->mountpoint(), DirectoryEntry::Type::Directory));
        }
    }

    // if(path.num_parts() == 0)
    //     return true;

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
    FileSystem* matching_fs = nullptr;
    size_t longest_mount_point_size = 0;
    for(auto* fs : mounted_filesystems) {
        ASSERT(fs != nullptr);
        if((fs->mountpoint().is_prefix_of(path)) && (fs->mountpoint().num_parts() >= longest_mount_point_size))
        {
            matching_fs = fs;
            longest_mount_point_size = fs->mountpoint().num_parts();
        }
    }

    if(matching_fs == nullptr)
        return nullptr;

    int rc = path.remove_mount_prefix(*matching_fs, path_inside_fs);
    ASSERT(rc);
    return matching_fs;
}


bool VFS::create_entry(const Path& path, DirectoryEntry::Type type)
{
    Path inside_path("/");
    FileSystem* fs = get_fs(path, inside_path);
    if(fs == nullptr)
        return false;
    return fs->create_entry(inside_path, type);
}