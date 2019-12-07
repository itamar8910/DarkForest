#include "unistd.h"
#include "stdio.h"
#include "asserts.h"
#include "mman.h"
#include "string.h"
#include "malloc.h"
#include "types/vector.h"
#include "types/String.h"

#include "PS2KeyboardCommon.h"

void process_command(const String& command) {
    auto parts = command.split(' ');
    if(parts.size() > 0) {
        auto program = parts[0];
        /**
         * fork and execute 'program'
         * program should be configured so that its stdout is the VgaTTY
         */
        puts((String("\n") + program).c_str());
    }
    puts("\n# ");
}


int main() {
    printf("# ");
    Vector<char> command(50);
    while(1) {
        char c = getchar();
        if(c == '\n') {
            process_command(String(command.data(), command.size()));
            command.clear();
        }
		else {
            putc(c);
            command.append(c);
		}
    }
    return 0;
}