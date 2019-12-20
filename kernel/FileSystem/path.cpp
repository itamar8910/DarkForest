#include "path.h"
#include "logging.h"

String Path::remove_mount_prefix(const String& path, 
                            const FileSystem& fs) {
    if(path.len() <= fs.mountpoint().len()) {
        return "";
    }
    if(path.startswith(fs.mountpoint()) && path[fs.mountpoint().len()] == '/') {
        return path.substr(fs.mountpoint().len() + 1);
    }
    return "";
}