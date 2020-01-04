// Based on: james molloy's tutorials & SerenityOS


#include "string.h"
#include "cpu.h"
#include "logging.h"
#include "types.h"
#include "Scheduler.h"
#include "backtrace.h"

#define GENERATE_BACKTRACE

TSS the_tss;

extern "C" void gdt_flush(uint32_t); // arg is ptr to descriptor ptr struct
extern "C" void idt_flush(uint32_t);// arg is ptr to descriptor ptr struct
extern "C" void tss_flush(uint32_t);// arg is idx of tss in GDT, lowest 2 bits are RPL

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

asm(
   ".globl tss_flush\n"
   "tss_flush:\n"
   "mov 4(%esp), %eax\n"
   "ltr %ax\n"
   "ret\n"
);

// Internal function prototypes.
static void init_gdt();
static void init_idt();
// static void idt_write(uint8_t,uint32_t,uint16_t,uint8_t);

#define NUM_GDT_ENTRIES 6
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

   // setup TSS entry
   memset(&the_tss, 0, sizeof(TSS));
   the_tss.ss0 = 0x10;
   the_tss.esp0 = 0; // will be set later before a task goes to ring3
   gdt_register_entry_raw(5, (uint32_t)&the_tss, sizeof(TSS), 0x89, 0x40); // User mode data segment (Ring3)

   gdt_flush((uint32_t)&gdt_ptr);
   // 0x28 because TSS entry is the 6th entry in the GDT (each entry is 8 bytes)
   // lower 2 bits = 3 = RPL
   tss_flush(0x28 | 3);
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

static void idt_register_entry_raw(uint8_t num, uint32_t base, bool user_allowed, uint16_t sel=0x08, uint8_t flags=0x8e)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   if(user_allowed){
      flags |= 0x60;  // set the DPL to 3
   }
   idt_entries[num].flags   = flags ;
} 

// hang cpu
[[noreturn]] void cpu_hang() {
   asm volatile("cli; hlt");
   while(1);
}

static void unimpl_trap() {
   kprint("unimplemented trap");
   cpu_hang();
}



UNHANDLED_EXCEPTION(0, "Division by 0") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(1, "Debug exception") 
UNHANDLED_EXCEPTION(2, "Unknown error")
UNHANDLED_EXCEPTION(3, "Breakpoint")
UNHANDLED_EXCEPTION(4, "Overflow")
UNHANDLED_EXCEPTION(5, "Bounds check")
UNHANDLED_EXCEPTION(6, "Illegal Instruction") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(7, "No Math Coprocessor")
// UNHANDLED_EXCEPTION(8, "Double fault")
UNHANDLED_EXCEPTION(9, "Coprocessor segment overrun")
UNHANDLED_EXCEPTION(10, "Invalid TSS")
UNHANDLED_EXCEPTION(11, "Segment not present")
UNHANDLED_EXCEPTION(12, "Stack exception")
// UNHANDLED_EXCEPTION(13, "General protection fault") // TODO: dont crash if happened in uermode
UNHANDLED_EXCEPTION(15, "Unknown error")
UNHANDLED_EXCEPTION(16, "Coprocessor error")

template <typename RegsDump>
void print_register_dump(RegsDump& regs) {
   kprintf(
      "Register Dump:\n"
      "edi: 0x%x\n"
      "esi: 0x%x\n"
      "ebp: 0x%x\n"
      "esp: 0x%x\n"
      "ebx: 0x%x\n"
      "edx: 0x%x\n"
      "ecx: 0x%x\n"
      "eax: 0x%x\n"
      "eip: 0x%x\n"
      "user esp: 0x%x\n",
      regs.edi,
      regs.esi,
      regs.ebp,
      regs.esp,
      regs.ebx,
      regs.edx,
      regs.ecx,
      regs.eax,
      regs.eip,
      regs.useresp
      );
}


template<typename T>
void kernel_panic(T regs) {
   #ifdef GENERATE_BACKTRACE
   Backtrace::print_backtrace(regs.eip, regs.ebp);
   #else
   (void)regs;
   #endif
   kprintf("K E R N E L P A N I C\n");
   cpu_hang();
}


// Page fault
ISR_EXCEPTION_WITH_ERRCODE(14);
void isr_14_handler(RegisterDumpWithErrCode& regs) {
   (void)regs;
   kprint("##### Page Fault #####\n");
   kprintf("Address that generated Fault: 0x%x\n", get_cr2());
   print_register_dump(regs);
   if(is_selector_ring3(regs.ds)) {
      kprintf("(From userspace)\n");
      Scheduler::the().terminate_current();
   } else {
      kprintf("(From kernel)\n");
      kernel_panic(regs);
   }
}

// General protection fault
ISR_EXCEPTION_WITH_ERRCODE(13);
void isr_13_handler(RegisterDumpWithErrCode& regs) {
   (void)regs;
   // TODO: don't crash in usermode
   kprint("*** General Protection Fault\n");
   print_register_dump(regs);
   cpu_hang();
}

// Double fault
// NOTE: for some reason this crashes if we
// make this a hadnler WITH_ERRCODE,
// but works if its NO_ERRCODE
// but #DF exception should push an error code of 0
ISR_EXCEPTION_NO_ERRCODE(8);
void isr_8_handler(RegisterDump& regs) {
   (void)regs;
   kprint("*** Double fault\n");
   print_register_dump(regs);
   // kprintf("Register dump: eax: ")
   // TODO: handle page fault
   // cpu_hang(); // until we implement
}

template <int N>
void invoke_trap() {
   asm volatile(
      "int %0\n"
      ::"N"(N)
      );
}

void invoke_crash() {
   invoke_trap<ISR_CRASH_IDX>();
}

ISR_HANDLER(crash)
void isr_crash_handler(RegisterDump& regs) {
   kernel_panic(regs);
}

void register_interrupt_handler(int num, void (*func)(), bool user_allowed) {
   idt_register_entry_raw(num, (uint32_t)func, user_allowed);
}

static void init_idt() {
   idt_ptr.base = (uint32_t) &idt_entries;
   idt_ptr.limit = NUM_IDT_ENTRIES*sizeof(IdtEntry) - 1;
   memset((void*)idt_ptr.base, 0, idt_ptr.limit);

   for(int i = 1; i < NUM_IDT_ENTRIES; i++) {
      register_interrupt_handler(i, unimpl_trap);
   }

   register_interrupt_handler(0, isr_0_entry);
   register_interrupt_handler(1, isr_1_entry);
   register_interrupt_handler(2, isr_2_entry);
   register_interrupt_handler(3, isr_3_entry);
   register_interrupt_handler(4, isr_4_entry);
   register_interrupt_handler(5, isr_5_entry);
   register_interrupt_handler(6, isr_6_entry);
   register_interrupt_handler(7, isr_7_entry);
   register_interrupt_handler(8, isr_8_entry);
   register_interrupt_handler(9, isr_9_entry);
   register_interrupt_handler(10, isr_10_entry);
   register_interrupt_handler(11, isr_11_entry);
   register_interrupt_handler(12, isr_12_entry);
   register_interrupt_handler(13, isr_13_entry);
   register_interrupt_handler(14, isr_14_entry);
   register_interrupt_handler(15, isr_15_entry);
   register_interrupt_handler(16, isr_16_entry);

   register_interrupt_handler(ISR_CRASH_IDX, isr_crash_entry);

   // register_interrupt_handler(0x50, isr_15_entry);


   idt_flush((uint32_t)&idt_ptr);

}

