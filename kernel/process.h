#pragma once
#include "task.h"
#include "file.h"

#define NUM_FILE_DESCRIPTORS 256

class Process {
public:
    static Process* create(void (*main)(), String name="[Unnamed]");

    Process(u32 pid, ThreadControlBlock* task, String name)
        : m_pid(pid),
          m_task(task),
          m_name(name) {}

    ThreadControlBlock& task() {return *m_task;}
    u32 pid(){return m_pid;}
    String name(){return m_name;}

    ~Process();

    int syscall_open(const String& path);
    int syscall_ioctl(size_t fd, u32 code, void* data);

private:
    u32 m_pid;
    ThreadControlBlock* m_task;
    String m_name;
    File* m_file_descriptors[NUM_FILE_DESCRIPTORS] {0};
};