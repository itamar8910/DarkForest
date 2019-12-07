#include "process.h"
#include "task.h"
#include "FileSystem/VFS.h"
#include "errs.h"
#include "device.h"
#include "FileSystem/CharFile.h"
#include "Loader/loader.h"
#include "Scheduler.h"

u32 g_next_pid;

Process* Process::create(void (*main)(), 
                        String name,
                        File** descriptors) {
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
    if(m_waiter) {
        m_waiter->waitee_finished();
    }
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

UserspaceLoaderData userspace_loader_data;
bool glob_userspace_loader_locked = false;

static void auxiliary_loader() {
    glob_userspace_loader_locked = false;
    load_and_jump_userspace(userspace_loader_data);
}

void copy_into_loader_data(UserspaceLoaderData& data, const char* path, char** argv, size_t argc)
{
    // TODO: copy
    // we need to do this because that data is in one process' userspace memory
    // and needs to be accesses from another
    // so we copy it to a temp location in kernel space
    strncpy(userspace_loader_data.glob_load_path, path, MAX_PATH_LEN);
    clone_args(data.argv, data.argc, argv, argc);
    // if(argc == 0) {
    //     data.argv = nullptr;
    //     data.argc = 0;
    // } else {
    //     data.argv = new char*[argc];
    //     for(size_t i = 0; i < argc; ++i) {
    //         data.argv[i] = new char[strlen(argv[i]) + 1];
    //         strcpy(data.argv[i], argv[i]);
    //     }
    //     data.argc = argc;
    // }
}

// int Process::syscall_ForkAndExec(char* path, char* name, char** argv)
int Process::syscall_ForkAndExec(ForkArgs* args)
{
    // not realy atomic
    // TODO: use a mutex here
    ASSERT(!glob_userspace_loader_locked);
    ASSERT(args != nullptr);
    glob_userspace_loader_locked = true;
    copy_into_loader_data(userspace_loader_data, args->path, args->argv, args->argc);
    auto* p = Process::create(auxiliary_loader, 
                                args->name,
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