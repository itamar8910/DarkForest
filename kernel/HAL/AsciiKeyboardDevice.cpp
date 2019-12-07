#include "KeyboardDevice.h"
#include "TaskBlocker.h"
#include "drivers/PS2Keyboard.h"
#include "Scheduler.h"
#include "asserts.h"
#include "HAL/KeyboardDevice.h"
#include "HAL/AsciiKeyboardDevice.h"


int AsciiKeyboardDevice::read(size_t count, void* buf) {
    ASSERT(count == 1);
    KeyEvent e;
    do {
        e = keyboard_read();
    } while(e.released || e.to_ascii() == 0);
    ((char*)buf)[0] = e.to_ascii();
    return 1;
}

int AsciiKeyboardDevice::write(char* data, size_t count) {
    (void)data;
    (void)count;
    NOT_IMPLEMENTED();
    return -1;
}

int AsciiKeyboardDevice::ioctl(u32 request, void* buf) {
    (void)request;
    (void)buf;
    NOT_IMPLEMENTED();
    return -1;

}
