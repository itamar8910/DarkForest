#include "MemoryManager.h"
#include "types/vector.h"

class SharedMemoryManager
{
private:
	SharedMemoryManager() = default; // singelton
	~SharedMemoryManager() = default;

public:
	static SharedMemoryManager& the();

public:
    struct SharedMemoryEntry
    {
        u32 guid;
        u32 pid;
        void* virt_addr;
        u32 size;
    };

public:
	bool register_shm(u32 guid, u32 pid, void* virt_addr, u32 size);
    bool get(u32 guid, SharedMemoryEntry&);


private:
    Vector<SharedMemoryEntry> m_entries;
};