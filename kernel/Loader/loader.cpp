#include "loader.h"
#include "types.h"
#include "MM/MemoryManager.h"
#include "string.h"
#include "Loader/ElfParser.h"
#include "bits.h"

#define USERSPACE_STACK 0xb0000000



void load_and_jump_userspace(void* elf_data, u32 size) {
   Elf::Header* header = (Elf::Header*) elf_data;
   Elf::ParseInfo elf_parse_info = Elf::from(elf_data, size);
   for(auto& offset : elf_parse_info.program_header_offsets) {
         Elf::ProgramHeader* program_header = (Elf::ProgramHeader*) ((u32)elf_data+offset);
         if(program_header->type != Elf::ProgramHeaderType::LOAD) {
            continue;
         }
         // I think we can assume this
         ASSERT(program_header->alignment == PAGE_SIZE, "elf program header elignment != PAGE_SIZE");
         void* segment_data = (void*) ((u32)elf_data + program_header->offset);
         void* segment_virtual_address = (void*) (program_header->virtual_address);
         u32 segment_mem_size = program_header->size_in_memory;
         ASSERT((u32)segment_virtual_address % PAGE_SIZE == 0, "elf segment virtual address is not aligned to PAGE_SIZE");
         bool writable = get_bit(program_header->flags, Elf::ProgramHeaderFlags::Writable);
         u32 page_index = 0;
         for(u32 size = 0; size < segment_mem_size; size += PAGE_SIZE, ++page_index) {
            MemoryManager::the().allocate((u32)segment_virtual_address + page_index*PAGE_SIZE,
            writable ? PageWritable::YES : PageWritable::NO,
            UserAllowed::YES
            );
         }
         // copy data from file
         memcpy(segment_virtual_address, segment_data, program_header->size_in_file);
         // zero leftover
         memset((void*)((u32)segment_virtual_address + program_header->size_in_file), 0, (page_index*PAGE_SIZE) - program_header->size_in_file);
   }
   // allocate stack
	u32 user_stack_bottom = USERSPACE_STACK;
	u32 user_stack_top = user_stack_bottom - PAGE_SIZE;
	u32* user_esp = (u32*) (user_stack_bottom - 16);
	MemoryManager::the().allocate(user_stack_top, PageWritable::YES, UserAllowed::YES);
   // jumpp to entry
   jump_to_usermode((void (*)())(header->entry), (u32) user_esp);
}

asm(
   ".globl jump_to_usermode\n"
   "jump_to_usermode:\n"
   "mov 4(%esp), %ebx\n" // arg1 = func to run in usermode
   "mov 8(%esp), %ecx\n" // arg2 = usermode esp
   "mov $0x23, %ax\n" // usermode data offset in gdt = 0x20, last 2 bits for RPL=3
   "mov %ax, %ds\n"
   "mov %ax, %es\n"
   "mov %ax, %fs\n"
   "mov %ax, %gs\n"
   "mov %ecx, %eax\n"
   "pushl $0x23\n" // stack segment
   "pushl %eax\n" // iret esp
   "pushf\n"
   "pushl $0x1b\n" // usermode code offset in gdt = 0x18, last 2 bits for RPL=3
   "pushl %ebx\n"
   "iret\n"
);