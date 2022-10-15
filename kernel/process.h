#pragma once
#include "task.h"
#include "file.h"
#include "fork_args.h"
#include "FileSystem/DirectoryEntry.h"
#include "types/list.h"
#include "lock.h"

#define NUM_FILE_DESCRIPTORS 256

class Process {
public:
    static Process* create(void (*main)(), 
                        String name="[Unnamed]",
                        String current_directory="/",
                        File** descriptors = nullptr);

    Process(u32 pid, ThreadControlBlock* task, String name, String current_directory, File** descriptors=nullptr);

    ThreadControlBlock& task() {return *m_task;}
    u32 pid(){return m_pid;}
    String name(){return m_name;}
    void set_waiter(WaitBlocker*);

    bool has_pending_message();

    ~Process();

    int syscall_open(const String& path);
    int syscall_ioctl(size_t fd, u32 code, void* data);
    int syscall_file_size(size_t fd);
    int syscall_read(size_t fd, char* buff, size_t count);
    int syscall_write(size_t fd, char* buff, size_t count);
    int syscall_ForkAndExec(ForkArgs*);
    int syscall_wait(size_t pid);
    int syscall_listdir(const String& path, void* dest, size_t* size);
    int syscall_set_current_directory(const String& path);
    int syscall_get_current_directory(char* buff, size_t* count);
    int syscall_creste_entry(const String& path, DirectoryEntry::Type type);
    int syscall_create_shared_memory(const u32 guid, const u32 size, void** addr);
    int syscall_open_shared_memory(const u32 guid, void** addr, u32* size);
    int syscall_send_message(const u32 pid, const char* msg, u32 size);
    int syscall_get_message(char* msg, u32 size, u32* pid);
    int syscall_get_pid_by_name(char* name, u32* pid);
    int syscall_map_device(int fd, void* addr, u32 size);
    int syscall_block_until_pending(u32* fds, u32 num_fds, u32* ready_fd_idx);
    int syscall_create_terminal(char* name_out);
    int syscall_lseek(int fd, int offset, int whence);
    int syscall_has_pending_message();
    int syscall_socket(int domain, int type, int protocol);

private:
    struct Message
    {
        u32 pid;
        char* message;
        u32 size;

        Message& operator=(const Message& other) = default;
    };

    bool get_message(Message& msg);
    int consume_message(char* msg, u32 size, u32* pid);

    void put_message(const char* msg, u32 size, u32 pid);


private:
    String get_full_path(const String& path);
    int allocate_fd(File*);

private:
    u32 m_pid;
    ThreadControlBlock* m_task;
    String m_name;
    String m_current_directory;
    void* m_next_shared_memory;

    File* m_file_descriptors[NUM_FILE_DESCRIPTORS] {0};
    WaitBlocker* m_waiter {nullptr};

    List<Message> m_messages {};
    Lock m_message_lock = {"message_lock"};
};
