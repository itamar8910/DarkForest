#pragma once

#include "types/String.h"
// #include "FileSystem.h"

class FileSystem;

class Path {

public:
    enum class PathType
    {
        Absolute,
        Relative,
        Invalid,
    };

    static Path empty();

    explicit Path(const String& path);
    explicit Path(const Vector<String>& parts, PathType);

    /**
     * e.g (/dev/keyboard, DevFS) => keyboard
     */
    bool remove_mount_prefix(const FileSystem& fs, Path& res) const;

    size_t num_parts() const {return m_parts.size();}
    const String& get_part(size_t idx) const {return m_parts[idx];}
    PathType type() const {return m_type;}

    Vector<String> parts_from_string(const String& path) const;
    PathType type_from_string(const String& path) const;
    bool is_prefix_of(const Path& other) const;
    bool is_parent_of(const Path& other) const;

    String to_string() const;
    String base_name() const; // returns last part
    Path dirname() const; // returns all but last part

    void add_part(const String& part);

private:
    Vector<String> m_parts;
    PathType m_type;
};