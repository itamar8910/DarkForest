#include "KeyboardDevice.h"
#include "TaskBlocker.h"
#include "Scheduler.h"
#include "asserts.h"
#include "MouseDevice.h"

bool MouseReadBlocker::can_unblock() {
    return PS2Mouse::the().can_consume();
}

MouseEvent MouseDevice::mouse_read() {
    MouseReadBlocker* blocker = new MouseReadBlocker();
    Scheduler::the().block_current(blocker);

    return PS2Mouse::the().consume();
}

int MouseDevice::read(size_t count, void* buf) {
    ASSERT(count == 1);
    MouseEvent e = mouse_read();
    new(buf) MouseEvent(e);
    return 1;
}

int MouseDevice::write(char* data, size_t count) {
    (void)data;
    (void)count;
    NOT_IMPLEMENTED();
    return -1;
}

int MouseDevice::ioctl(u32 request, void* buf) {
    (void)request;
    (void)buf;
    NOT_IMPLEMENTED();
    return -1;
}

bool MouseDevice::can_read() const
{
    return PS2Mouse::the().can_consume();
}