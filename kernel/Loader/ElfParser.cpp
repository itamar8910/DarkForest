#include "ElfParser.h"
#include "types.h"
#include "logging.h"
#include "asserts.h"

Elf::ParseInfo Elf::from(void* data, u32 size) {
    (void)size;
    Elf::Header* header = (Elf::Header*)data;
    kprintf("elf magic: 0x%x\n", header->magic);
    ASSERT(header->magic == 0x464c457f);
    ASSERT(header->bitclass == 1);
    ASSERT(header->endianess == 1);
    ASSERT(header->elf_version == 1);
    ASSERT(header->type == Elf::ElfType::EXEC);
    ASSERT(header->header_size == 52);
    ASSERT(header->program_header_table_entry_size==32);
    Vector<u32> program_header_offsets;
    for(size_t i = 0; i < header->num_program_headers; i++) {
        program_header_offsets.append(
            header->program_header_offset 
            + (i * header->program_header_table_entry_size)
        );
    }
    ParseInfo info;
    info.program_header_offsets = program_header_offsets;
    return info;
}