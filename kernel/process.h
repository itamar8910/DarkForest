#pragma once
#include "task.h"
#include "file.h"

#define NUM_FILE_DESCRIPTORS 256

class Process {
public:
    static Process* create(void (*main)(), String name="[Unnamed]", File** descriptors = nullptr);

    Process(u32 pid, ThreadControlBlock* task, String name, File** descriptors=nullptr);

    ThreadControlBlock& task() {return *m_task;}
    u32 pid(){return m_pid;}
    String name(){return m_name;}
    void set_waiter(WaitBlocker*);

    ~Process();

    int syscall_open(const String& path);
    int syscall_ioctl(size_t fd, u32 code, void* data);
    int syscall_file_size(size_t fd);
    int syscall_read(size_t fd, char* buff, size_t count);
    int syscall_write(size_t fd, char* buff, size_t count);
    int syscall_ForkAndExec(char* path);
    int syscall_wait(size_t pid);

private:
    u32 m_pid;
    ThreadControlBlock* m_task;
    String m_name;
    File* m_file_descriptors[NUM_FILE_DESCRIPTORS] {0};
    WaitBlocker* m_waiter {nullptr};
};