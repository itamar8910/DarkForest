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

bool SharedMemoryManager::register_shm(u32 guid, u32 pid, void* virt_addr, u32 size)
{
    for(auto& entry : m_entries)
    {
        if(entry.guid == guid)
        {
            return false;
        }
    }
    m_entries.append({guid, pid, virt_addr, size});
    return true;
}


bool SharedMemoryManager::get(u32 guid, SharedMemoryManager::SharedMemoryEntry& res)
{
    for(auto& entry : m_entries)
    {
        if(entry.guid == guid)
        {
            res = entry;
            return true;
        }
    }
    return false;
}