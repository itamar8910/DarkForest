// Based on:
// http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
// https://github.com/conmarap/osdev-tutorial/blob/master/4-gdt.md

// descriptor_tables.c - Initialises the GDT and IDT, and defines the 
// default ISR and IRQ handler.
// Based on code from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "Cstring.h"
#include "cpu_descriptors.h"


// declare functions that are implemented in ASM
extern "C" void gdt_flush(uint32_t);
extern "C" void idt_flush(uint32_t);
// declare all isr{i} entires
extern "C" void isr0();
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

// Internal function prototypes.
static void init_gdt();
static void gdt_write(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);
static void init_idt();
static void idt_write(uint8_t,uint32_t,uint16_t,uint8_t);

#define NUM_GDT_ENTRIES 5
#define NUM_IDT_ENTRIES 256

gdt_entry_t gdt_entries[NUM_GDT_ENTRIES];
gdt_ptr_t gdt_ptr;
idt_entry_t idt_entries[NUM_IDT_ENTRIES];
idt_ptr_t   idt_ptr;

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables()
{
   // Initialise the global descriptor table.
   init_gdt();
   init_idt();
}


static void init_gdt()
{
   gdt_ptr.limit = (sizeof(gdt_entry_t) * NUM_GDT_ENTRIES) - 1;
   gdt_ptr.base  = (uint32_t)&gdt_entries;

   gdt_write(0, 0, 0, 0, 0);                // Null segment
   gdt_write(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code segment (Ring0)
   gdt_write(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data segment (Ring0)
   gdt_write(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment (Ring3)
   gdt_write(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment (Ring3)

   gdt_flush((uint32_t)&gdt_ptr);
}

// Set the value of one GDT entry.
static void gdt_write(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
   gdt_entries[num].base_low    = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low   = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access      = access;
}

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
   idt_entries[num].flags   = flags /* | 0x60 */;
} 