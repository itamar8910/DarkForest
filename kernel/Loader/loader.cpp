#include "loader.h"
#include "types.h"
#include "MM/MemoryManager.h"
#include "string.h"
#include "Loader/ElfParser.h"
#include "bits.h"
#include "types/String.h"
#include "file.h"
#include "FileSystem/FileUtils.h"
#include "FileSystem/VFS.h"
#include "errs.h"

#define USERSPACE_STACK 0xb0000000

static int read_elf_from_path(const String& path, shared_ptr<Vector<u8>>& elf_data, size_t& elf_size)
{
	File* f = VFS::the().open(Path(path));
   if(f == nullptr) {
      kprintf("elf at path: %s was not found!\n", path.c_str());
      return E_NOTFOUND;
   }
	elf_data = FileUtils::read_all(*static_cast<CharFile*>(f), elf_size);
   if(elf_data.get() == nullptr) {
      return E_INVALID;
   }
   elf_data->set_size(elf_size);
   return 0;
}

void load_and_jump_userspace(const String& path) {
   kprintf("load_and_jump_userspace: %s\n", path.c_str());
	size_t elf_size = 0;
   shared_ptr<Vector<u8>> elf_data;
   int rc = read_elf_from_path(path, elf_data, elf_size);
   kprintf("rc: %d\n", rc);
   ASSERT(rc == 0);
   ASSERT(elf_data.get() != nullptr);
   kprintf("a1\n");
   print_hexdump(elf_data->data(), 0x100);
	load_and_jump_userspace(elf_data, elf_size);
}

void load_and_jump_userspace(UserspaceLoaderData& data) {
	size_t elf_size = 0;
   shared_ptr<Vector<u8>> elf_data;
   int rc = read_elf_from_path(data.glob_load_path, elf_data, elf_size);
   ASSERT(rc == 0);
   load_and_jump_userspace(elf_data, elf_size, data.argv, data.argc);
}

static void* allocate_from_buffer(size_t len, void*& current_alloc_buff) {
   kprintf("allocate_from_buffer with len: %d\n", len);
   void* t = current_alloc_buff;
   current_alloc_buff = (void*)((u32)current_alloc_buff + len);
   kprintf("current_alloc_buffer res: 0x%x\n", t);
   return t;
}
static void clone_args_into_userspace(char**& argv_dst, size_t& argc_dst, char** argv_src, size_t argc_src, void* argv_mem_start)
{
    if(argc_src == 0) {
        argv_dst = nullptr;
        argc_dst = 0;
    } else {
         argv_dst = (char**) allocate_from_buffer(argc_src * sizeof(char*), argv_mem_start);
        for(size_t i = 0; i < argc_src; ++i) {
            argv_dst[i] = (char*) allocate_from_buffer((strlen(argv_src[i]) + 1)*sizeof(char), argv_mem_start);
            strcpy(argv_dst[i], argv_src[i]);
        }
        argc_dst = argc_src;
    }
}

void load_and_jump_userspace(shared_ptr<Vector<u8>> elf_data_ptr,
                                u32 size,
                                char** argv,
                                size_t argc)
{
   u8* elf_data = elf_data_ptr->data();
   ASSERT(elf_data != nullptr);
   Elf::Header* header = (Elf::Header*) elf_data;
   Elf::ParseInfo elf_parse_info = Elf::from(elf_data, size);
   for(auto& offset : elf_parse_info.program_header_offsets) {
         Elf::ProgramHeader* program_header = (Elf::ProgramHeader*) ((u32)elf_data+offset);
         if(program_header->type != Elf::ProgramHeaderType::LOAD) {
            continue;
         }
         // I think we can assume this
         ASSERT(program_header->alignment == PAGE_SIZE);
         void* segment_data = (void*) ((u32)elf_data + program_header->offset);
         void* segment_virtual_address = (void*) (program_header->virtual_address);
         u32 segment_mem_size = program_header->size_in_memory;
         ASSERT((u32)segment_virtual_address % PAGE_SIZE == 0);
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

   u32 entry = header->entry;
   // NOTE: we have to call force_free
   // becuase we jump to userspace, control call destructor of shared_ptr
   elf_data_ptr.force_free();

   // allocate stack
	u32 user_stack_bottom = USERSPACE_STACK;
	u32 user_stack_top = user_stack_bottom - PAGE_SIZE;
	u32* user_esp = (u32*) (user_stack_bottom - 16);
	MemoryManager::the().allocate(user_stack_top, PageWritable::YES, UserAllowed::YES);
   // allocate args(argc, argv)
   u32 user_args_end = USERSPACE_STACK + PAGE_SIZE*2;
   u32 user_args_start = user_args_end - PAGE_SIZE;
	MemoryManager::the().allocate(user_args_start, PageWritable::YES, UserAllowed::YES);
   char*** dst_argv = (char***)(user_args_start+4);
   size_t* dst_argc_ptr = (size_t*)(user_args_start+8);
   clone_args_into_userspace(*dst_argv, *dst_argc_ptr, argv, argc, (void*)(user_args_start+16));

   kprintf("jump_to_usermode: entry: 0x%x\n", entry);
   // jumpp to entry
   jump_to_usermode((void (*)())(entry), (u32) user_esp);
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

void clone_args(char**& argv_dst, size_t& argc_dst, char** argv_src, size_t argc_src)
 {
    if(argc_src == 0) {
        argv_dst = nullptr;
        argc_dst = 0;
    } else {
        argv_dst = new char*[argc_src];
        for(size_t i = 0; i < argc_src; ++i) {
            argv_dst[i] = new char[strlen(argv_src[i]) + 1];
            strcpy(argv_dst[i], argv_src[i]);
        }
        argc_dst = argc_src;
    }
 }