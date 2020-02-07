#include "MM/SharedMemoryManager.h"
#include "errs.h"


static SharedMemoryManager* s_the = nullptr;
SharedMemoryManager& SharedMemoryManager::the()
{
    if(!s_the) {
        s_the = new SharedMemoryManager();
    }
    return *s_the;
}

int SharedMemoryManager::register_shared_memory(u32 guid, u32 pid)
{
    for(auto& entry : m_entries)
    {
        if(entry.guid == guid)
        {
            return E_TAKEN;
        }
    }
    m_entries.append({guid, pid});
    return E_OK;
}


u32 SharedMemoryManager::get_pid_of_shared_memory(u32 guid)
{
    for(auto& entry : m_entries)
    {
        if(entry.guid == guid)
        {
            return entry.pid;
        }
    }
    return 0;
}