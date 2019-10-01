// Based on: james molloy's tutorials & SerenityOS


#include "Cstring.h"
#include "cpu_descriptors.h"
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
   extern "C" void isr_exception_##idx() { \
      kprint(msg "\n"); \
      hang(); \
   }

ISR_EXCEPTION_NO_ERRCODE(3);
void isr_exception_3_handler(RegisterDump& regs) {
   (void)regs; // to suppress unused
   kprint("exception3  handler\n");
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

   register_interrupt_handler(3, isr_exception_3_entry);


   idt_flush((uint32_t)&idt_ptr);

}

/*
static void idt_set_all_entries() {
    idt_write( 0, (uint32_t)isr0 , 0x08, 0x8E);
    idt_write( 1, (uint32_t)isr1 , 0x08, 0x8E);
    idt_write( 2, (uint32_t)isr2 , 0x08, 0x8E);
    idt_write( 3, (uint32_t)isr3 , 0x08, 0x8E);
    idt_write( 4, (uint32_t)isr4 , 0x08, 0x8E);
    idt_write( 5, (uint32_t)isr5 , 0x08, 0x8E);
    idt_write( 6, (uint32_t)isr6 , 0x08, 0x8E);
    idt_write( 7, (uint32_t)isr7 , 0x08, 0x8E);
    idt_write( 8, (uint32_t)isr8 , 0x08, 0x8E);
    idt_write( 9, (uint32_t)isr9 , 0x08, 0x8E);
    idt_write( 10, (uint32_t)isr10 , 0x08, 0x8E);
    idt_write( 11, (uint32_t)isr11 , 0x08, 0x8E);
    idt_write( 12, (uint32_t)isr12 , 0x08, 0x8E);
    idt_write( 13, (uint32_t)isr13 , 0x08, 0x8E);
    idt_write( 14, (uint32_t)isr14 , 0x08, 0x8E);
    idt_write( 15, (uint32_t)isr15 , 0x08, 0x8E);
    idt_write( 16, (uint32_t)isr16 , 0x08, 0x8E);
    idt_write( 17, (uint32_t)isr17 , 0x08, 0x8E);
    idt_write( 18, (uint32_t)isr18 , 0x08, 0x8E);
    idt_write( 19, (uint32_t)isr19 , 0x08, 0x8E);
    idt_write( 20, (uint32_t)isr20 , 0x08, 0x8E);
    idt_write( 21, (uint32_t)isr21 , 0x08, 0x8E);
    idt_write( 22, (uint32_t)isr22 , 0x08, 0x8E);
    idt_write( 23, (uint32_t)isr23 , 0x08, 0x8E);
    idt_write( 24, (uint32_t)isr24 , 0x08, 0x8E);
    idt_write( 25, (uint32_t)isr25 , 0x08, 0x8E);
    idt_write( 26, (uint32_t)isr26 , 0x08, 0x8E);
    idt_write( 27, (uint32_t)isr27 , 0x08, 0x8E);
    idt_write( 28, (uint32_t)isr28 , 0x08, 0x8E);
    idt_write( 29, (uint32_t)isr29 , 0x08, 0x8E);
    idt_write( 30, (uint32_t)isr30 , 0x08, 0x8E);
    idt_write( 31, (uint32_t)isr31 , 0x08, 0x8E);
}

static void init_idt()
{
   idt_ptr.limit = sizeof(idt_entry_t) * NUM_IDT_ENTRIES -1;
   idt_ptr.base  = (uint32_t)&idt_entries;

    // make all entries in IDT NULL entries
    memset(&idt_entries, 0, sizeof(idt_entry_t)*NUM_IDT_ENTRIES);

    idt_set_all_entries();

    idt_flush((uint32_t)&idt_ptr);
}

static void idt_write(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags ;
} 

*/