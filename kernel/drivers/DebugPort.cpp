
#include "DebugPort.h"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void DebugPort::write(const char* data, size_t size) {
    for(size_t i = 0; i < size; i++) {
        outb(DEBUG_PORT_ADDR, data[i]);
    }
}

void DebugPort::write(const char* str) {
    DebugPort::write(str, strlen(str));
}

void DebugPort::write_dec(int) {
    // TODO: impl
    DebugPort::write("DebugPort::write_dec: implement me!");
}