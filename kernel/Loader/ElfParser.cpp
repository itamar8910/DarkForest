#include "ElfParser.h"
#include "types.h"
#include "logging.h"
#include "Kassert.h"

Elf::ParseInfo Elf::from(void* data, u32 size) {
    (void)size;
    Elf::Header* header = (Elf::Header*)data;
    kprintf("elf magic: 0x%x\n", header->magic);
    ASSERT(header->magic == 0x464c457f, "elf header magic error");
    ASSERT(header->bitclass == 1, "elf header must be 32 bit");
    ASSERT(header->endianess == 1, "endianess must be little");
    ASSERT(header->elf_version == 1, "elf version must be 1");
    ASSERT(header->type == Elf::ElfType::EXEC, "elf type must be executable");
    ASSERT(header->header_size == 52, "invalid elf header size");
    ASSERT(header->program_header_table_entry_size==32, "invalid program header size in elf header");
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