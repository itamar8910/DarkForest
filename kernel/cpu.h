#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "types.h"

// Based on:
// http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
// https://github.com/conmarap/osdev-tutorial/blob/master/4-gdt.md

struct [[gnu::packed]] DescriptorTablePtr
{
   uint16_t limit;
   uint32_t base;
};

struct [[gnu::packed]] GdtEntry
{
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           // The last 8 bits of the base.
};
static_assert (sizeof(GdtEntry) == 8); // 64 bit



// A struct describing an interrupt gate.
struct [[gnu::packed]] IdtEntry
{
   uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16_t sel;                 // Kernel segment selector.
   uint8_t  always0;             // This must always be zero.
   uint8_t  flags;               // More flags. See documentation.
   uint16_t base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
static_assert (sizeof(IdtEntry) == 8); // 64 bit


struct [[gnu::packed]] RegisterDump
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

struct [[gnu::packed]] RegisterDumpWithErrCode
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t err_code;
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

void init_descriptor_tables();

[[noreturn]] void cpu_hang();

u32 get_cr3();

u32 get_cr2();