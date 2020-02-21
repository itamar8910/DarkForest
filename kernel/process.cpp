#include "process.h"
#include "task.h"
#include "FileSystem/VFS.h"
#include "errs.h"
#include "device.h"
#include "FileSystem/CharFile.h"
#include "Loader/loader.h"
#include "Scheduler.h"
#include "MM/SharedMemoryManager.h"
#include "Math.h"

u32 g_next_pid;

Process* Process::create(void (*main)(), 
                        String name,
                        String current_directory,
                        File** descriptors) {
    auto* task = create_kernel_task(main);
    return new Process(g_next_pid++, task, name, current_directory, descriptors);
}

Process::Process(u32 pid, ThreadControlBlock* task, String name, String current_directory, File** descriptors)
    : m_pid(pid),
      m_task(task),
      m_name(name),
      m_current_directory(current_directory),
      m_next_shared_memory((void*)SHARED_MEMORY_START)
{
    if(descriptors) {
        for(size_t i = 0; i < NUM_FILE_DESCRIPTORS; i++) {
            m_file_descriptors[i] = descriptors[i];
        }
    } else {
        for(size_t i = 0; i < NUM_FILE_DESCRIPTORS; i++) {
            m_file_descriptors[i] = nullptr;
        }

    }
}

Process::~Process() {
    // TODO: free usespace resources & pages
    if(m_waiter) {
        m_waiter->waitee_finished();
    }
    delete m_task;
}

int Process::syscall_open(const String& path) {


    auto* file = VFS::the().open(Path(get_full_path(path)));
    if(file == nullptr)
        return -E_NOTFOUND;
    for(size_t i = 0; i < NUM_FILE_DESCRIPTORS; i++) {
        if(m_file_descriptors[i] == nullptr) {
            m_file_descriptors[i] = file;
            return i;
        }
    }
    return -E_LIMIT;
}

int Process::syscall_ioctl(size_t fd, u32 code, void* data) {
    if(fd >= NUM_FILE_DESCRIPTORS)
        return -E_NOTFOUND;
    auto* file = m_file_descriptors[fd];
    if(file == nullptr)
        return -E_NOTFOUND;
    // TODO: dynamic_cast
    Device* d = static_cast<Device*>(file);
    return d->ioctl(code, data);
}

int Process::syscall_file_size(size_t fd) {
    if(fd >= NUM_FILE_DESCRIPTORS)
        return -E_NOTFOUND;
    auto* file = m_file_descriptors[fd];
    if(file == nullptr)
        return -E_NOTFOUND;
    // TODO: dynamic_cast
    CharFile* cf = static_cast<CharFile*>(file);
    return cf->size();

}

int Process::syscall_read(size_t fd, char* buff, size_t count) {
    if(fd >= NUM_FILE_DESCRIPTORS)
        return -E_NOTFOUND;
    auto* file = m_file_descriptors[fd];
    if(file == nullptr)
        return -E_NOTFOUND;
    return file->read(count, buff);
}

int Process::syscall_write(size_t fd, char* buff, size_t count) {
    if(fd >= NUM_FILE_DESCRIPTORS)
        return -E_NOTFOUND;
    auto* file = m_file_descriptors[fd];
    if(file == nullptr)
    {
        return -E_NOTFOUND;
    }
    return file->write(buff, count);

}

UserspaceLoaderData userspace_loader_data;
bool glob_userspace_loader_locked = false;

static void auxiliary_loader() {
    glob_userspace_loader_locked = false;
    load_and_jump_userspace(userspace_loader_data);
}

void copy_into_loader_data(UserspaceLoaderData& data, const char* path, char** argv, size_t argc)
{
    // we need to do this because that data is in one process' userspace memory
    // and needs to be accesses from another
    // so we copy it to a temp location in kernel space
    strncpy(userspace_loader_data.glob_load_path, path, MAX_PATH_LEN);
    clone_args(data.argv, data.argc, argv, argc);
}

// int Process::syscall_ForkAndExec(char* path, char* name, char** argv)
int Process::syscall_ForkAndExec(ForkArgs* args)
{
    kprintf("ForkAndExec: %s\n", args->path);
    u32 free_space, num_blocks, num_pages;
    KernelHeapAllocator::the().heap_statistics(free_space, num_blocks, num_pages);
    kprintf("### Heap: free: %d, #blocks: %d, #pages: %d\n", free_space, num_blocks, num_pages);
    // not realy atomic
    // TODO: use a mutex here
    ASSERT(!glob_userspace_loader_locked);
    ASSERT(args != nullptr);
    glob_userspace_loader_locked = true;
    copy_into_loader_data(userspace_loader_data, args->path, args->argv, args->argc);
    auto* p = Process::create(auxiliary_loader, 
                                args->name,
                                m_current_directory, /* child inherits current directory */
                                m_file_descriptors);
    int pid = p->pid();
    Scheduler::the().add_process(p);
    return pid;
    
}
int Process::syscall_wait(size_t pid)
{
    auto* waitee = Scheduler::the().get_process(pid);
    if(waitee==nullptr) {
        return -E_NOTFOUND;
    }
    auto* blocker = new WaitBlocker();
    waitee->set_waiter(blocker);
    Scheduler::the().block_current(blocker);
    return 0;
}

void Process::set_waiter(WaitBlocker* blocker)
{
    ASSERT(m_waiter==nullptr);
    m_waiter = blocker;
}

int Process::syscall_listdir(const String& path, void* dest, size_t* size)
{
    kprintf("syscall_listdir for path: %s\n", path.c_str());
    if(size==nullptr)
        return E_INVALID;


    Vector<DirectoryEntry> entries;
    bool rc = VFS::the().list_directory(Path(get_full_path(path)), entries);
    if(!rc)
    {
        kprintf("listdir: not found\n");
        return E_NOTFOUND;
    }

    kprintf("listdir # entries: %d\n", entries.size());
    size_t required_size = 0;
    for(auto& entry : entries)
    {
        required_size += entry.serialize(nullptr);
    }
    if((*size < required_size) || (dest == nullptr))
    {
        *size = required_size;
        return E_TOO_SMALL;
    }

    size_t offset = 0;
    for(auto& entry : entries)
    {
        u8* current = ((u8*)dest) + offset;
        offset += entry.serialize(current);
    }
    return 0;
}

int Process::syscall_set_current_directory(const String& path)
{
    if (!VFS::the().is_directory(Path(get_full_path(path))))
    {
        kprintf("Not setting current directory of %s since it doesn't exist\n", path.c_str());
        return E_NOTFOUND;
    }

    if (path.find_last_of('/') == (int)(path.len() - 1))
    {
        kprintf("Setting directory with path ending with / is not supported yet\n");
        return E_INVALID;
    }

    m_current_directory = get_full_path(path);

    kprintf("Set current directory of %s\n", m_current_directory.c_str());

    return 0;
}

int Process::syscall_get_current_directory(char* buff, size_t* count)
{
    if (*count < m_current_directory.len())
    {
        *count = m_current_directory.len();
        return E_TOO_SMALL;
    }
    
    memcpy(buff, m_current_directory.c_str(), *count);

    return 0;
}

String Process::get_full_path(const String& path)
{
    // TODO: Do this better, perhaps add a get_current_directory call, or that every directory will have a "." file that will represent the directory
    if (path == String("."))
    {
        return m_current_directory;
    }

    if (path == String(".."))
    {
        int last_delimiter_pos = m_current_directory.find_last_of('/');

        if (last_delimiter_pos == 0)
        {
            return '/';
        }

        return m_current_directory.substr(0, last_delimiter_pos);
    }

    if (path[0] == '/')
    {
        return path;
    }

    if (m_current_directory[m_current_directory.len() - 1] == '/')
    {
        return m_current_directory + path;
    }

    return m_current_directory + String('/') + path;
}

int Process::syscall_creste_entry(const String& path, DirectoryEntry::Type type)
{
    Path fullpath = Path(get_full_path(path));
    const bool rc = VFS::the().create_entry(fullpath, type);
    if(rc == false)
    {
        return -E_INVALID;
    }
    return 0;
}

int Process::syscall_create_shared_memory(const u32 guid, const u32 size, void** addr)
{
    if(((size%PAGE_SIZE) != 0) || ((u32)m_next_shared_memory + size >= SHARED_MEMORY_END))
    {
        return E_INVALID_SIZE;
    }

    const int rc = MemoryManager::the().allocate_range((u32)m_next_shared_memory, size, PageWritable::YES, UserAllowed::YES);
    if(rc)
    {
        return rc;
    }

    const bool register_rc = SharedMemoryManager::the().register_shm(guid, m_pid, m_next_shared_memory, size);
    if(!register_rc)
    {
        return E_TAKEN;
    }

    *addr = m_next_shared_memory;
    m_next_shared_memory = reinterpret_cast<void*>((u32)m_next_shared_memory + size);
    return E_OK;
}

int Process::syscall_open_shared_memory(const u32 guid, void** addr, u32* size)
{
    SharedMemoryManager::SharedMemoryEntry entry;
    const bool rc = SharedMemoryManager::the().get(guid, entry);
    if(!rc)
    {
        return E_NOTFOUND;
    }

    if(entry.pid == m_pid)
    {
        *addr = entry.virt_addr;
        *size = entry.size;
        return E_OK;
    }

    Process* owner = Scheduler::the().get_process(entry.pid);
    if(!owner)
    {
        kprintf("shared memory owner not found\n");
        return E_NOTFOUND;
    }
    const int dup_rc = MemoryManager::the().duplicate(owner->m_task->CR3, (u32)entry.virt_addr, entry.size, (u32)m_next_shared_memory);
    if(dup_rc)
    {
        return dup_rc;
    }
    *addr = m_next_shared_memory;
    *size = entry.size;

    m_next_shared_memory = reinterpret_cast<void*>((u32)m_next_shared_memory + entry.size);

    return E_OK;
}

int Process::syscall_send_message(const u32 pid, const char* msg, u32 size)
{
    Process* receiver = Scheduler::the().get_process(pid);
    if(!receiver)
    {
        kprintf("notfound\n");
        return E_NOTFOUND;
    }

    receiver->put_message(msg, size, m_pid); 
    return E_OK;
}

int Process::syscall_get_message(char* msg, u32 size, u32* pid)
{
    if(has_pending_message())
    {
        return consume_message(msg, size, pid);
    }

    kprintf("no messages, blocking..\n");

    auto* blocker = new PendingMessageBlocker(m_pid);
    Scheduler::the().block_current(blocker);

    return consume_message(msg, size, pid);
}

int Process::consume_message(char* msg, u32 size, u32* pid)
{
    Message m;
    const bool rc  = get_message(m);
    ASSERT(rc);

    if(size != m.size)
    {
        // TODO: handle this case,
        // TODO take 'size' bytes from the message & truncate it
        ASSERT_NOT_REACHED();
    }
    const u32 size_to_copy = Math::min(size, m.size);

    memcpy(msg, m.message, size_to_copy);
    *pid = m.pid;

    delete[] m.message;

    return size_to_copy;
}

bool Process::has_pending_message()
{
    return !m_messages.empty();
}

bool Process::get_message(Message& msg)
{
    LOCKER(m_message_lock);
    if(m_messages.empty())
    {
        return false;
    }
    msg = m_messages.pop_front();
    kprintf("get message: 0x%x\n", msg);
    return true;
}

void Process::put_message(const char* msg, u32 size, u32 pid)
{
    LOCKER(m_message_lock);
    kprintf("put message:");
    print_hexdump((const u8*)msg, size);
    char* msg_copy = new char[size];
    memcpy(msg_copy, msg, size);
    m_messages.append({pid, msg_copy, size});
}


int Process::syscall_get_pid_by_name(char* name, u32* pid)
{
    Process* p = Scheduler::the().get_process_by_name(name);
    if(!p)
    {
        return E_NOTFOUND;
    }
    *pid = p->pid();
    return E_OK;
}


int Process::syscall_map_device(int fd, void* addr, u32 size)
{
    if(fd >= NUM_FILE_DESCRIPTORS)
        return -E_NOTFOUND;
    auto* f = m_file_descriptors[fd];
    if(f == nullptr)
        return -E_NOTFOUND;
    Device* dev = static_cast<Device*>(f); // TODO dynamic cast
    return dev->mmap(addr, size);
}

int Process::syscall_block_until_pending(u32* fds, u32 num_fds, u32* ready_fd_idx)
{
    Vector<File*> pending_files;
    for(u32 i = 0; i < num_fds; ++i)
    {
        if(fds[i] >= NUM_FILE_DESCRIPTORS)
        {
            return -E_INVALID;
        }
        auto* f = m_file_descriptors[fds[i]];
        if(f == nullptr)
        {
            return -E_NOTFOUND;
        }
        pending_files.append(f);
    } 

    PendingInputBlocker* blocker = new PendingInputBlocker(m_pid, pending_files);
    Scheduler::the().block_current(blocker);
    kprintf("PendingInputBlocker returned, ready_fd_idx: %d, reason:%d\n", blocker->ready_fd_idx(), blocker->reason());
    const u32 ready_fd_idx_in_array = blocker->ready_fd_idx();
    ASSERT(ready_fd_idx_in_array < num_fds);
    *ready_fd_idx = fds[ready_fd_idx_in_array];
    return static_cast<int>(blocker->reason());
}
