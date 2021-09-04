
#include "df_unistd.h"
#include "asserts.h"
#include "stdio.h"

void initialize_descriptors() {
    int keyboard_fd = std::open("/dev/asciiKeyboard");
    int vga_tty_fd = std::open("/dev/vgatty");
    ASSERT(keyboard_fd == STDIN);
    ASSERT(vga_tty_fd == STDOUT);
    // TODO: STDERR ?
}

void print_hello_text() {
    int fd = std::open("/init/hello.txt");
    ASSERT(fd>=0);
    int size = std::file_size(fd);
    ASSERT(size > 0);
    char* buff = new char[size+1];
    int rc = std::read(fd, buff, size);
    ASSERT(rc == size);
    buff[size] = 0;
    printf("%s\n", buff);
}

int main() {
    initialize_descriptors();
    print_hello_text();
    int pid = std::fork_and_exec("/bin/shell.app", "shell");
    ASSERT(pid > 0);
    std::wait(pid);
    return 0;
}
