#pragma once
#include "types.h"
#include "types/vector.h"

namespace Elf
{
    enum ElfType : u16 {
        None = 0,
        REL = 1,
        EXEC = 2,
        DYN = 3,
        CORE = 4,
    };
    enum BitClass : u8 {
        thiryTwoBit = 1,
        sixyFourbit = 2,
    };
    enum Endianess : u8 {
        Little = 1,
        Big = 2,
    };
    struct [[gnu::packed]] Header {
        u32 magic; // 0x7F ELF
        BitClass bitclass; // 32/64
        Endianess endianess; // 1=little, 2=big
        u8 elf_version; // must be 1
        u8 os_abi; // ignored
        u8 abi_version; // ignore
        u8 pad[7]; // unused
        ElfType type;
        u16 machine;
        u32 elf_version2;
        u32 entry;
        u32 program_header_offset;
        u32 section_header_offset;
        u32 flags;
        u16 header_size;
        u16 program_header_table_entry_size;
        u16 num_program_headers;
        u16 section_header_table_entry_size;
        u16 num_section_headers;
        u16 str_section_header_index;
    };
    static_assert(sizeof(Header) == 0x34);

    enum ProgramHeaderType : u32{
        PT_NULL = 0,
        LOAD = 1,
        DYNAMIC = 2,
        INTERP = 3,
        NOTE = 4,
    };

    enum ProgramHeaderFlags {
        Executable = 1,
        Writable = 2,
        Readable = 4,
    };

    struct [[gnu::packed]] ProgramHeader{
        ProgramHeaderType type;
        u32 offset;
        u32 virtual_address;
        u32 physical_address; // not relevant
        u32 size_in_file;
        u32 size_in_memory;
        u32 flags;
        u32 alignment;
    };
    static_assert(sizeof(ProgramHeader) == 0x20);

    struct ParseInfo {
        Vector<u32> program_header_offsets;
    };


    ParseInfo from(void* data, u32 size);

}
