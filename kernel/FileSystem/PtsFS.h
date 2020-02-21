
#pragma once

#include "FileSystem.h"
#include "device.h"
#include "types/vector.h"
#include "Pipe.h"

constexpr u32 MAX_TERMINALS = 16;

/**
 * Psuedo-Terminals filesystem
 * Each terminal is a pair of pipes: stdin & stdout
 */
class PtsFS: public FileSystem {
public:
    static PtsFS& the();

    File* open(const Path& path) override;
    bool list_directory(const Path& path, Vector<DirectoryEntry>& res) override;
    bool is_directory(const Path& path) override;
    bool is_file(const Path& path) override;
    bool create_entry(const Path& path, DirectoryEntry::Type type) override;

    bool create_new(String& name);

struct Terminal
{
    Pipe* stdin;
    Pipe* stdout;
};

private:
    PtsFS() : FileSystem(Path("/dev/pts")) {}
    Terminal m_terminals[MAX_TERMINALS] {};
    bool used_map[MAX_TERMINALS] {false};
};
