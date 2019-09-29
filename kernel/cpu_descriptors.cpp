// Based on:
// http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
// https://github.com/conmarap/osdev-tutorial/blob/master/4-gdt.md

// descriptor_tables.c - Initialises the GDT and IDT, and defines the 
// default ISR and IRQ handler.
// Based on code from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "cpu_descriptors.h"

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

// Lets us access our ASM functions from our C code.
extern "C" void gdt_flush(uint32_t);

// Internal function prototypes.
static void init_gdt();
static void gdt_write(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);

// idt_entry_t idt_entries[256];
// idt_ptr_t   idt_ptr;

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables()
{
   // Initialise the global descriptor table.
   init_gdt();
}

#define NUM_GDT_ENTRIES 5

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