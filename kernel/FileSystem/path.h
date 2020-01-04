#pragma once

#include "types/String.h"
#include "FileSystem.h"

class Path {

public:
    enum class PathType
    {
        Absolute,
        Relative,
        Invalid,
    };

    Path(const String& path);

    /**
     * e.g (/dev/keyboard, DevFS) => keyboard
     */
    static String remove_mount_prefix(const String& path, 
                            const FileSystem& fs);
    
    size_t num_parts() const {return m_parts.size();}
    const String& get_part(size_t idx) const {return m_parts[idx];}
    PathType type() const {return m_type;}

    Vector<String> parts_from_string(const String& path) const;
    PathType type_from_string(const String& path) const;

    String to_string() const;

private:
    Vector<String> m_parts;
    PathType m_type;
};