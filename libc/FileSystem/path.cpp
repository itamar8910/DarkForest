#include "path.h"
#include "logging.h"
#include "kernel/FileSystem/FileSystem.h"


Path::Path(const String& path)
    : m_parts(parts_from_string(path)),
      m_type(type_from_string(path))
{
}

Path::Path(const Vector<String>& parts, PathType type)
:   m_parts(parts),
    m_type(type)
{
}

bool Path::remove_mount_prefix(const FileSystem& fs, Path& res) const
{
    auto mount_point = fs.mountpoint();
    if(num_parts() < mount_point.num_parts()) {
        return false;
    }

    for(size_t i = 0; i < mount_point.num_parts(); ++i)
    {
        if(get_part(i) != mount_point.get_part(i))
            return false;
    }

    Vector<String> parts;
    for(size_t i = mount_point.num_parts(); i < num_parts(); ++i)
    {
        parts.append(get_part(i));
    }
    res = Path(parts, PathType::Relative);
    return true;
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

String Path::base_name() const
{
    ASSERT(num_parts() != 0);
    return m_parts[m_parts.size()-1];
}

Path Path::dirname() const
{
    if(num_parts() < 2)
    {
        return Path("/");
    }
    return Path(m_parts.range(0, m_parts.size() - 1), m_type);
}