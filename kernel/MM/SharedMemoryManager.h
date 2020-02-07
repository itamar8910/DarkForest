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
	int register_shared_memory(u32 guid, u32 pid);
    u32 get_pid_of_shared_memory(u32 guid);

private:
    struct SharedMemoryEntry
    {
        u32 guid;
        u32 pid;
    };

private:
    Vector<SharedMemoryEntry> m_entries;
};