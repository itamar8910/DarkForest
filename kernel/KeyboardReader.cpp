#include "KeyboardReader.h"
#include "Scheduler.h"
#include "drivers/PS2Keyboard.h"

bool KeyboardReadBlocker::can_unblock() {
    return PS2Keyboard::the().can_consume();
}

KeyEvent keyboard_read() {
    KeyboardReadBlocker* blocker = new KeyboardReadBlocker();
    Scheduler::the().block_current(blocker);

    return PS2Keyboard::the().consume();

}
