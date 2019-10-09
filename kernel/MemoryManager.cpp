
#include "MemoryManager.h"
#include "logging.h"
#include "assert.h"
#include "string.h"
#include "bits.h"

/*
    Virtual Memory map:
    (0MB-4MB - identity mapped)
    3MB - 4MB: kmalloc
    0xc0000000 - 0xffffffff : kernel address space
*/

static MemoryManager* mm = nullptr;

void MemoryManager::initialize(multiboot_info_t* mbt) {
    kprintf("MemoryManager::initialize()\n");
    mm = new MemoryManager();
    mm->init(mbt);
}

void MemoryManager::init(multiboot_info_t* mbt) {
    kprintf("Physical memory map:");
    // loop over all mmap entries
	for(
        MultibootMemMapEntry* mmap = (MultibootMemMapEntry*) mbt->mmap_addr
        ; (u32) mmap < mbt->mmap_addr + mbt->mmap_length
        ; mmap = (MultibootMemMapEntry*) ( (unsigned int)mmap + mmap->size + sizeof(mmap->size) )
         ) {
        kprintf("base: 0x%x%08x, len: 0x%x%08x, type: %d\n",
            u32((mmap->base >> 32) & 0xffffffff),
            u32(mmap->base & 0xffffffff),
            u32((mmap->len >> 32) & 0xffffffff),
            u32(mmap->len & 0xffffffff),
            u32(mmap->type)
        );
        if(mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        ASSERT(
            u32((mmap->base >> 32) & 0xffffffff) == 0
            && u32((mmap->len >> 32) & 0xffffffff) == 0
            , "physical memory not accessible with 32bit"
        );
        u32 region_base = u32(mmap->base & 0xffffffff);
        u32 region_len = u32(mmap->len & 0xffffffff);
        for(u32 frame_base = region_base; frame_base <= region_base+region_len-PAGE_SIZE; frame_base += PAGE_SIZE ) {
            set_frame_available(PhysicalAddress(frame_base));
        }
	}
}


MemoryManager& the() {
    ASSERT(mm != nullptr, "MemoryManager is uninitialized");
    return *mm;
}

MemoryManager::MemoryManager() { 
    memset(m_frames_avail_bitmap, 0, sizeof(m_frames_avail_bitmap));
    memset(m_frames_free_bitmap, 0, sizeof(m_frames_free_bitmap));
}

MemoryManager::~MemoryManager() {
    ASSERT(false, "MM should not be destructed");
}