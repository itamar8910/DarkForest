// Based on: james molloy's tutorials & SerenityOS


#include "string.h"
#include "cpu.h"
#include "DebugPort.h"
#include "logging.h"


extern "C" void gdt_flush(uint32_t); // arg is ptr to descriptor ptr struct
extern "C" void idt_flush(uint32_t);// arg is ptr to descriptor ptr struct

asm(
   ".globl gdt_flush\n"
   "gdt_flush:\n"
   "mov 4(%esp), %eax\n"
   "lgdt (%eax)\n"
   "mov $0x10, %ax\n" // load kernel DS into segments registors
   "mov %ax, %ds\n"
   "mov %ax, %es\n"
   "mov %ax, %fs\n"
   "mov %ax, %gs\n"
   "mov %ax, %ss\n"
   "ljmpl $0x8, $dummy\n" // this "far jump" implicitly sets CS to the kernel's
   "dummy:\n"
   "ret\n"
);

asm(
   ".globl idt_flush\n"
   "idt_flush:\n"
   "mov 4(%esp), %eax\n"
   "lidt (%eax)\n"
   "ret\n"
);

// Internal function prototypes.
static void init_gdt();
static void init_idt();
// static void idt_write(uint8_t,uint32_t,uint16_t,uint8_t);

#define NUM_GDT_ENTRIES 5
#define NUM_IDT_ENTRIES 256

GdtEntry gdt_entries[NUM_GDT_ENTRIES];
IdtEntry idt_entries[NUM_IDT_ENTRIES];
DescriptorTablePtr gdt_ptr;
DescriptorTablePtr idt_ptr;

void init_descriptor_tables()
{
   // Initialise the global descriptor table.
   init_gdt();
   init_idt();
}

static void gdt_register_entry_raw(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

static void init_gdt()
{
   gdt_ptr.limit = (sizeof(GdtEntry) * NUM_GDT_ENTRIES) - 1;
   gdt_ptr.base  = (uint32_t)&gdt_entries;

   gdt_register_entry_raw(0, 0, 0, 0, 0);                // Null segment
   gdt_register_entry_raw(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code segment (Ring0)
   gdt_register_entry_raw(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data segment (Ring0)
   gdt_register_entry_raw(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment (Ring3)
   gdt_register_entry_raw(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment (Ring3)

   gdt_flush((uint32_t)&gdt_ptr);
}

static void gdt_register_entry_raw(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
}

static void idt_register_entry_raw(uint8_t num, uint32_t base, uint16_t sel=0x08, uint8_t flags=0x8e)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags ;
} 

// hang cpu
[[noreturn]] static void hang() {
   asm volatile("cli; hlt");
   while(1);
}

static void unimpl_trap() {
   DebugPort::write("unimplemented trap");
   hang();
}

// exception handler for exception without error codes
#define ISR_EXCEPTION_NO_ERRCODE(idx) \
   extern "C" void isr_exception_##idx##_handler(RegisterDump&); \
   extern "C" void isr_exception_##idx##_entry(); \
   asm( \
      ".globl isr_exception_" #idx "_entry\n" \
      "isr_exception_" #idx "_entry:\n" \
      "pusha\n"\
      "mov %ds, %ax # lower 16 bits of eax = ds \n"  \
      "pushl %eax\n" \
      "mov $0x10, %ax # load kernel data segment descriptor\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "pushl %esp # arg is ref to RegisterDump\n"\
      "call isr_exception_" #idx "_handler\n"\
      "addl $0x4, %esp\n"\
      "popl %eax # reload original DS\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "popa\n"\
      "iret\n"\
   );


// exception handler for exception with error codes
#define ISR_EXCEPTION_WITH_ERRCODE(idx) \
   extern "C" void isr_exception_##idx##_handler(RegisterDumpWithErrCode&); \
   extern "C" void isr_exception_##idx##_entry(); \
   asm( \
      ".globl isr_exception_" #idx "_entry\n" \
      "isr_exception_" #idx "_entry:\n" \
      "pusha\n"\
      "mov %ds, %ax # lower 16 bits of eax = ds \n"  \
      "pushl %eax\n" \
      "mov $0x10, %ax # load kernel data segment descriptor\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "pushl %esp # arg is ref to RegisterDump\n"\
      "call isr_exception_" #idx "_handler\n"\
      "addl $0x4, %esp\n"\
      "popl %eax # reload original DS\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "popa\n"\
      "addl $0x4, %esp\n"\
      "iret\n"\
   );

#define UNHANDLED_EXCEPTION(idx, msg) \
   static void isr_exception_##idx##_entry() { \
      kprint(msg "\n"); \
      hang(); \
   }

UNHANDLED_EXCEPTION(0, "Division by 0") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(1, "Debug exception") 
UNHANDLED_EXCEPTION(2, "Unknown error")
UNHANDLED_EXCEPTION(3, "Breakpoint")
UNHANDLED_EXCEPTION(4, "Overflow")
UNHANDLED_EXCEPTION(5, "Bounds check")
UNHANDLED_EXCEPTION(6, "Illegal Instruction") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(7, "No Math Coprocessor")
UNHANDLED_EXCEPTION(8, "Double fault")
UNHANDLED_EXCEPTION(9, "Coprocessor segment overrun")
UNHANDLED_EXCEPTION(10, "Invalid TSS")
UNHANDLED_EXCEPTION(11, "Segment not present")
UNHANDLED_EXCEPTION(12, "Stack exception")
UNHANDLED_EXCEPTION(13, "General protection fault") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(15, "Unknown error")
UNHANDLED_EXCEPTION(16, "Coprocessor error")

// Page fault
ISR_EXCEPTION_WITH_ERRCODE(14);
void isr_exception_14_handler(RegisterDumpWithErrCode& regs) {
   (void)regs;
   kprint("Page fault\n");
   // TODO: handle page fault
   hang(); // until we implement
}

static void register_interrupt_handler(int num, void (*func)()) {
   idt_register_entry_raw(num, (uint32_t)func);
}

static void init_idt() {
   idt_ptr.base = (uint32_t) &idt_entries;
   idt_ptr.limit = NUM_IDT_ENTRIES*sizeof(IdtEntry) - 1;

   for(int i = 0; i < NUM_IDT_ENTRIES; i++) {
      register_interrupt_handler(i, unimpl_trap);
   }

   register_interrupt_handler(0, isr_exception_0_entry);
   register_interrupt_handler(1, isr_exception_1_entry);
   register_interrupt_handler(2, isr_exception_2_entry);
   register_interrupt_handler(3, isr_exception_3_entry);
   register_interrupt_handler(4, isr_exception_4_entry);
   register_interrupt_handler(5, isr_exception_5_entry);
   register_interrupt_handler(6, isr_exception_6_entry);
   register_interrupt_handler(7, isr_exception_7_entry);
   register_interrupt_handler(8, isr_exception_8_entry);
   register_interrupt_handler(9, isr_exception_9_entry);
   register_interrupt_handler(10, isr_exception_10_entry);
   register_interrupt_handler(11, isr_exception_11_entry);
   register_interrupt_handler(12, isr_exception_12_entry);
   register_interrupt_handler(13, isr_exception_13_entry);
   register_interrupt_handler(14, isr_exception_14_entry);
   register_interrupt_handler(15, isr_exception_15_entry);
   register_interrupt_handler(16, isr_exception_16_entry);

   idt_flush((uint32_t)&idt_ptr);

}
