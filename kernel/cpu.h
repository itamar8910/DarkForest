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

void register_interrupt_handler(int num, void (*func)());


#define ISR_HANDLER(name) \
   extern "C" void isr_##name##_handler(RegisterDump&); \
   extern "C" void isr_##name##_entry(); \
   asm( \
      ".globl isr_" #name "_entry\n" \
      "isr_" #name "_entry:\n" \
      "pusha\n"\
      "mov %ds, %ax # lower 16 bits of eax = ds \n"  \
      "pushl %eax\n" \
      "mov $0x10, %ax # load kernel data segment descriptor\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "pushl %esp # arg is ref to RegisterDump\n"\
      "call isr_" #name "_handler\n"\
      "addl $0x4, %esp\n"\
      "popl %eax # reload original DS\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "popa\n"\
      "iret\n"\
   );

// exception handler for exception without error codes
#define ISR_EXCEPTION_NO_ERRCODE(name) ISR_HANDLER(name)

// exception handler for exception with error codes
#define ISR_EXCEPTION_WITH_ERRCODE(name) \
   extern "C" void isr_##name##_handler(RegisterDumpWithErrCode&); \
   extern "C" void isr_##name##_entry(); \
   asm( \
      ".globl isr_" #name "_entry\n" \
      "isr_" #name "_entry:\n" \
      "pusha\n"\
      "mov %ds, %ax # lower 16 bits of eax = ds \n"  \
      "pushl %eax\n" \
      "mov $0x10, %ax # load kernel data segment descriptor\n"\
      "mov %ax, %ds\n"\
      "mov %ax, %es\n"\
      "mov %ax, %fs\n"\
      "mov %ax, %gs\n"\
      "pushl %esp # arg is ref to RegisterDump\n"\
      "call isr_" #name "_handler\n"\
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
   static void isr_##idx##_entry() { \
      kprint(msg "\n"); \
      cpu_hang(); \
   }

