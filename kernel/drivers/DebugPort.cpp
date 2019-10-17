
#include "DebugPort.h"
#include "IO.h"


void DebugPort::putc(char c) {
    IO::outb(DEBUG_PORT_ADDR, c);
}

void DebugPort::write(const char* data, size_t size) {
    for(size_t i = 0; i < size; i++) {
        DebugPort::putc(data[i]);
    }
}

void DebugPort::write(const char* str) {
    DebugPort::write(str, strlen(str));
}
