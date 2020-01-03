
#pragma once
#include "types.h"

namespace IO {
    // hack that should wait for IO to finish
    static inline void wait() {
        asm volatile ( "jmp 1f\n\t"
                "1:jmp 2f\n\t"
                "2:" );
        }
    static inline void outb(uint16_t port, uint8_t val, bool wait = false) {
        asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
        if(wait) {
            IO::wait();
        }
    }

    static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile ( "inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }

    static inline uint16_t in16(uint16_t port) {
        uint16_t ret;
        asm volatile ( "inw %1, %0"
                    : "=a"(ret)
                    : "Nd"(port) );
        return ret;
    }
}