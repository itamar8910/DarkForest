#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"

#include "PS2KeyboardCommon.h"
#include "VgaTTY.h"

void print_hello_text() {
    int fd = open("/initrd/hello.txt");
    ASSERT(fd>=0);
    int size = file_size(fd);
    ASSERT(size > 0);
    char* buff = new char[size+1];
    int rc = read(fd, buff, size);
    ASSERT(rc == size);
    buff[size] = 0;
    VgaTTY::the().write(buff);
}

void process_command(const String& command) {
    (void) command;
    VgaTTY::the().write("\n# ");
}

int main() {
    print_hello_text();

    VgaTTY::the().write("# ");
    int keyboard_fd = open("/dev/keyboard");
    ASSERT(keyboard_fd != 0);
    KeyEvent key_event;
    Vector<char> command(50);
    while(1) {
        read(keyboard_fd, (char*) &key_event, 1);
        if(key_event.released)
            continue;
        if(key_event.to_ascii() == '\n') {
            process_command(String(command.data(), command.size()));
        }
		else if(key_event.to_ascii() != 0) {
			VgaTTY::the().putchar(key_event.to_ascii());
            command.append(key_event.to_ascii());
		}
    }
    
    return 0;
}