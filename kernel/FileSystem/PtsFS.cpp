#include "FileSystem/PtsFS.h"
static PtsFS* s_the = nullptr;

PtsFS& PtsFS::the()
{
    if(s_the == nullptr)
    {
        s_the = new PtsFS();
    }
    return *s_the;
}

File* PtsFS::open(const Path& path)
{
    kprintf("PtsFS::open %s\n", path.to_string().c_str());
    if(path.num_parts() != 2)
    {
        return nullptr;
    }

    const int idx = df_atoi(path.get_part(0).c_str(), 10);
    const String type = path.get_part(1); 

    if(!used_map[idx])
    {
        return nullptr;
    }

    Terminal terminal = m_terminals[idx];

    if(type == "in")
    {
        return terminal.stdin;
    }

    if(type == "out")
    {
        return terminal.stdout;
    }

    return nullptr;
}


bool PtsFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    (void)path;
    (void)res;
    return false;
}

bool PtsFS::is_directory(const Path& path)
{
    (void)path;
    return false;
}

bool PtsFS::is_file(const Path& path)
{
    (void)path;
    return false;
}

bool PtsFS::create_entry(const Path& path, DirectoryEntry::Type type)
{
    (void)path;
    (void)type;
    NOT_IMPLEMENTED();
}

bool PtsFS::create_new(String& name)
{
    for(u32 idx = 0; idx < MAX_TERMINALS; ++idx)
    {
        if(used_map[idx])
        {
            continue;
        }
        char max_idx_string[64];
        itoa(idx, max_idx_string, 64);
        String base = String("/dev/pts/") + String(max_idx_string);
        m_terminals[idx].stdin = new Pipe(Path(base + "in"), true);
        m_terminals[idx].stdout = new Pipe(Path(base + "out"), false);
        name = String(max_idx_string);
        used_map[idx] = true;
        return true;
    }
    return false;
}
