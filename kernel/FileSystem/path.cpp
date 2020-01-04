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

Path::Path(const String& path)
    : m_parts(parts_from_string(path)),
      m_type(type_from_string(path))
{
}

Vector<String> Path::parts_from_string(const String& path) const
{
    Vector<String> parts;
    size_t cur = 0;
    if(path.len() == 0)
    {
        return parts;
    }
    if(path[0] == '/')
    {
        cur = 1;
    }
    
    while(cur < path.len())
    {
        int next_sep = path.substr(cur).find("/");
        if(next_sep == -1)
        {
            parts.append(path.substr(cur));
            break;
        }
        next_sep += cur;
        parts.append(path.substr(cur, next_sep));
        cur = next_sep + 1;
    }
    return parts;
}

Path::PathType Path::type_from_string(const String& path) const
{
    if(path.len() == 0)
        return PathType::Invalid;
    if(path[0] == '/')
        return PathType::Absolute;
    return PathType::Relative;
}

String Path::to_string() const
{
    String res;
    for(size_t i = 0; i < num_parts(); ++i)
    {
        String part_i = get_part(i);
        res = res + part_i;
        if(i != num_parts() - 1)
        {
            res = res + String("/");
        }
    }
    return res;
}