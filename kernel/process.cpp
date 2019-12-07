#include "process.h"
#include "task.h"
#include "FileSystem/VFS.h"
#include "errs.h"
#include "device.h"
#include "FileSystem/CharFile.h"
#include "Loader/loader.h"
#include "Scheduler.h"

u32 g_next_pid;

Process* Process::create(void (*main)(), String name, File** descriptors) {
    auto* task = create_kernel_task(main);
    return new Process(g_next_pid++, task, name, descriptors);
}

Process::Process(u32 pid, ThreadControlBlock* task, String name, File** descriptors)
    : m_pid(pid),
      m_task(task),
      m_name(name)
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
    delete m_task;
}

int Process::syscall_open(const String& path) {
    auto* file = VFS::the().open(path);
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
        return -E_NOTFOUND;
    return file->write(buff, count);

}

String* glob_load_path = nullptr;
bool glob_load_locked = false;

static void auxiliary_loader() {
    ASSERT(glob_load_path != nullptr);
    load_and_jump_userspace(*glob_load_path);
    delete glob_load_path;
    glob_load_path = nullptr;
    glob_load_locked = false;
}

int Process::syscall_ForkAndExec(char* path)
{
    // not realy atomic
    // TODO: use a mutex here
    ASSERT(!glob_load_locked);
    glob_load_locked = true;
    // path is in userspace, we need to copy it to kernel space
    // because new process won't share the data of the previous one
    glob_load_path = new String(path);
    // TODO: add 'name' param to syscall
    Scheduler::the().add_process(Process::create(auxiliary_loader, "Unnamed", m_file_descriptors));
    return 0;
    
}