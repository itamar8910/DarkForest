
#include "MemoryManager.h"
#include "logging.h"

void MemoryManager::initialize(multiboot_info_t* mbt) {
    kprintf("MemoryManager::initialize()\n");
    kprintf("Physical memory map:");
	MultibootMemMapEntry* mmap = (MultibootMemMapEntry*) mbt->mmap_addr;
	while((u32) mmap < mbt->mmap_addr + mbt->mmap_length) {
        kprintf("base: 0x%x%08x, len: 0x%x%08x, type: %d\n",
            u32((mmap->base >> 32) & 0xffffffff),
            u32(mmap->base & 0xffffffff),
            u32((mmap->len >> 32) & 0xffffffff),
            u32(mmap->len & 0xffffffff),
            u32(mmap->type)
        );
		mmap = (MultibootMemMapEntry*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) );
	}
}