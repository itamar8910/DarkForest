
#include "MemoryManager.h"
#include "logging.h"
#include "Kassert.h"
#include "string.h"
#include "bits.h"
#include "PageTable.h"
#include "PageDirectory.h"


static MemoryManager* mm = nullptr;

void MemoryManager::initialize(multiboot_info_t* mbt) {
    kprintf("MemoryManager::initialize()\n");
    mm = new MemoryManager();
    mm->init(mbt);
}

void MemoryManager::init(multiboot_info_t* mbt) {
    mm->m_page_directory = new PageDirectory(PhysicalAddress(PageDirectory::get_cr3()));
    mm->m_tempmap_used = false;
    kprintf("Physical memory map:\n");
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
        kprintf("region base:%x\n", region_base);
        for(u32 frame_base = region_base; frame_base <= region_base+region_len-PAGE_SIZE; frame_base += PAGE_SIZE ) {
            // we don't want to allocate frames with base addr bellow 5MB
            if(frame_base < 5 * MB)
                continue;
            set_frame_available(PhysicalAddress(frame_base));
            ASSERT(is_frame_available(PhysicalAddress(frame_base)), "frame should be available");
        }
	}
}

Frame MemoryManager::get_free_frame(Err& err, bool set_used) {
    for(u32 i = 0; i < N_FRAME_BITMAP_ENTRIES; i++) {
        u32 avail_entry = m_frames_avail_bitmap[i];
        if(!avail_entry)
            continue;
        int set_bit_i = get_on_bit_idx(avail_entry);
        if(set_bit_i < 0)
            continue;
        err = 0;
        auto frame = Frame::from_bitmap_entry(BitmapEntry{i, (u32)set_bit_i});
        if(set_used) {
            set_frame_used(frame);
        }
        return frame;
    }
    err = ERR_NO_FREE_FRAMES;
    return 0;
}

void MemoryManager::set_frame_used(const Frame& frame) {
    auto bmp_entry = frame.get_bitmap_entry();
    set_bit(m_frames_avail_bitmap[bmp_entry.m_entry_idx], bmp_entry.m_entry_bit, false);

}

void MemoryManager::set_frame_available(Frame frame) {
    auto bitmap_entry = frame.get_bitmap_entry();
    set_bit(
        m_frames_avail_bitmap[bitmap_entry.m_entry_idx],
        bitmap_entry.m_entry_bit,
        true
    );
}

bool MemoryManager::is_frame_available(const Frame frame) {
    frame.assert_aligned();
    auto bitmap_entry = frame.get_bitmap_entry();
    return get_bit(
        m_frames_avail_bitmap[bitmap_entry.m_entry_idx],
        bitmap_entry.m_entry_bit
    );

}

VirtualAddress MemoryManager::temp_map(PhysicalAddress addr) {
    ASSERT(!m_tempmap_used, "TempMap is already used");
    auto pte = ensure_pte(TEMPMAP_ADDR, false, false);
    pte.set_addr(addr);
    pte.set_present(true);
    pte.set_writable(true);
    pte.set_user_allowed(false);
    flush_tlb(TEMPMAP_ADDR);
    m_tempmap_used = true;
    return TEMPMAP_ADDR;

}

void MemoryManager::un_temp_map() {
    auto pte = ensure_pte(TEMPMAP_ADDR, false, false);
    pte.set_addr(0);
    pte.set_present(false);
    pte.set_writable(false);
    pte.set_user_allowed(false);
    flush_tlb(TEMPMAP_ADDR);
    m_tempmap_used = false;
}

void MemoryManager::flush_tlb(VirtualAddress addr) {
    asm volatile("invlpg (%0)" ::"r" ((u32)addr) : "memory");
    // asm volatile("invlpg %0"
    //             :
    //             : "m"(addr)
    //             : "memory");
}
void MemoryManager::flush_entire_tlb()
{
    asm volatile(
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n" ::
            : "%eax", "memory");
}

PTE MemoryManager::ensure_pte(VirtualAddress addr, bool create_new_PageTable, bool tempMap_pageTable) {
    auto pde = m_page_directory->get_pde(addr);
    if(!pde.is_present() && create_new_PageTable) {
        kprintf("no PDE for virt addr: 0x%x, creating a new page table\n", addr);
        // we need to create a new page table
        Err err;
        Frame pt_frame = get_free_frame(err);
        ASSERT(!err, "could not allocate page table");
        // zero page table
        auto temp_vaddr = temp_map(pt_frame);
        memset((void*)(u32)temp_vaddr, 0, PAGE_SIZE);
        un_temp_map();

        pde.set_addr(pt_frame);
        pde.set_present(true);
        pde.set_writable(true);
        pde.set_user_allowed(true);

    }
    ASSERT(pde.is_present(), "page table not present");

    VirtualAddress pt_addr = pde.addr();
    if(tempMap_pageTable) {
        pt_addr = temp_map(pde.addr()); // map page table so we can access it
    }

    auto page_table = PageTable(pt_addr);

    return page_table.get_pte(addr);

}

void MemoryManager::allocate(VirtualAddress virt_addr, bool writable, bool user_allowed) {
    kprintf("MM: allocate: 0x%x\n", virt_addr);
    auto pte = ensure_pte(virt_addr);
    ASSERT(!pte.is_present(), "allocate: PTE already present for this virtual addr");
    Err err;
    Frame pt_frame = get_free_frame(err);
    ASSERT(!err, "could not allocate frame");
    pte.set_addr(pt_frame);
    pte.set_present(true);
    pte.set_writable(writable);
    pte.set_user_allowed(user_allowed);
    un_temp_map(); // page table of PTE is temp mapped
    flush_tlb(virt_addr);

}

MemoryManager& MemoryManager::the() {
    ASSERT(mm != nullptr, "MemoryManager is uninitialized");
    return *mm;
}

MemoryManager::MemoryManager() { 
    memset(m_frames_avail_bitmap, 0, sizeof(m_frames_avail_bitmap));
}

MemoryManager::~MemoryManager() {
    ASSERT(false, "MM should not be destructed");
}