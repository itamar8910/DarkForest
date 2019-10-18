#include "PIT.h"
#include "PIC.h"
#include "cpu.h"
#include "logging.h"
#include "IO.h"
#include "bits.h"
#include "Scheduler.h"

// based on serenity OS

#define IRQ_TIMER 0
// #define PRINT_SECONDS

static u32 s_ticks_this_second;
static u32 s_seconds_since_boot;

ISR_HANDLER(timer);
void isr_timer_handler(RegisterDump& regs) {
    (void)regs;
    if (++s_ticks_this_second >= TICKS_PER_SECOND) {
        ++s_seconds_since_boot;
        s_ticks_this_second = 0;
        #ifdef PRINT_SECONDS
        kprintf("Seconds: %d\n", s_seconds_since_boot);
        #endif
    }
    PIC::eoi(IRQ_TIMER);
    Scheduler::the().tick(regs);
}


namespace PIT {

u32 ticks_this_second() {
    return s_ticks_this_second;
}

u32 seconds_since_boot() {
    return s_seconds_since_boot;
}

void initialize() {
    kprintf("PIT::initialize\n");
    u16 timer_reload;

    IO::outb(PIT_CTL, TIMER0_SELECT | WRITE_WORD | MODE_SQUARE_WAVE);

    timer_reload = (BASE_FREQUENCY / TICKS_PER_SECOND);

    kprintf("PIT: %d Hz, square wave (%x)\n", TICKS_PER_SECOND, timer_reload);

    IO::outb(TIMER0_CTL, LSB(timer_reload));
    IO::outb(TIMER0_CTL, MSB(timer_reload));

    register_interrupt_handler(IRQ_ISRS_BASE + IRQ_TIMER, isr_timer_entry);

    PIC::enable_irq(IRQ_TIMER);
}

}
