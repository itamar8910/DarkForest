#include "KeyboardDevice.h"
#include "TaskBlocker.h"
#include "drivers/PS2Keyboard.h"
#include "Scheduler.h"
#include "asserts.h"


bool KeyboardReadBlocker::can_unblock() {
    return PS2Keyboard::the().can_consume();
}

KeyEvent keyboard_read() {
    KeyboardReadBlocker* blocker = new KeyboardReadBlocker();
    Scheduler::the().block_current(blocker);

    return PS2Keyboard::the().consume();
}

int KeyboardDevice::read(size_t count, void* buf) {
    ASSERT(count == 1);
    KeyEvent e = keyboard_read();
    // copy to void* buf
    new(buf) KeyEvent(e);
    return 1;
}

int KeyboardDevice::write(char* data, size_t count) {
    (void)data;
    (void)count;
    NOT_IMPLEMENTED();
    return -1;
}

int KeyboardDevice::ioctl(u32 request, void* buf) {
    (void)request;
    (void)buf;
    NOT_IMPLEMENTED();
    return -1;

}