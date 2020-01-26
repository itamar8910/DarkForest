
#include "MemoryManager.h"
#include "logging.h"
#include "asserts.h"
#include "string.h"
#include "bits.h"
#include "PageTable.h"
#include "PageDirectory.h"
#include "cpu.h"
#include "kmalloc.h"
// #include "lock.h"
#include "InterruptDisabler.h"
// #include "string.h"

// #define MM_DBG

static MemoryManager* mm = nullptr;

// Lock& get_memory_manager_lock()
// {
//     static Lock lock("MemoryManager");
//     return lock;
// }    

// TODO: use a bitmap for page directories instead of just advancing next_page_directory_allocation
static u32 next_page_directory_allocation = 2 * MB;
const u32 PAGE_DIRECTORY_ALLOCATION_END = next_page_directory_allocation + 1*MB;

void MemoryManager::initialize(multiboot_info_t* mbt) { kprintf("MemoryManager::initialize()\n");
    // LOCKER(get_memory_manager_lock());
    InterruptDisabler dis();
    mm = new MemoryManager();
    mm->init(mbt);
}

void MemoryManager::init(multiboot_info_t* mbt) {
    mm->m_page_directory = new PageDirectory(PhysicalAddress(get_cr3()));
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
        );
        u32 region_base = u32(mmap->base & 0xffffffff);
        u32 region_len = u32(mmap->len & 0xffffffff);
        kprintf("region base:%x\n", region_base);
        for(u32 frame_base = region_base; frame_base <= region_base+region_len-PAGE_SIZE; frame_base += PAGE_SIZE ) {
            // we don't want to allocate frames with base addr bellow 5MB
            if(frame_base < 5 * MB)
                continue;
            set_frame_available(PhysicalAddress(frame_base));
            ASSERT(is_frame_available(PhysicalAddress(frame_base)));
        }
	}
    disable_page(Frame(0)); // so we would page fault on null pointer deref
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
    // kprintf("MM:temp_map: 0x%x", addr);
    // kprintf("temp_map: 0x%x\n", (u32)addr);
    ASSERT(!m_tempmap_used);
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
}
void MemoryManager::flush_entire_tlb()
{
    asm volatile(
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n" ::
            : "%eax", "memory");
}

PTE MemoryManager::ensure_pte(VirtualAddress addr, bool create_new_PageTable, bool tempMap_pageTable) {
    // kprintf("MemoryManager::ensure_pte: 0x%x\n", (u32)addr);
    auto pde = m_page_directory->get_pde(addr);
    bool new_pagetable = false;
    if(!pde.is_present() && create_new_PageTable) {
        kprintf("no PDE for virt addr: 0x%x, creating a new page table\n", addr);
        
        if(address_in_kernel_space(addr)) {
            // see comment above lock_kernel_PDEs()
            // we do allow kernel PDEs to diverge in big buffer allocations
            if((addr < BIG_BUFFER_START) && (addr > BIG_BUFFER_END))
            {
                ASSERT(!m_kernel_PDEs_locked);
            }
        }

        // we need to create a new page table
        Err err;
        Frame pt_frame = get_free_frame(err);
        ASSERT(!err);
        new_pagetable = true;

        pde.set_addr(pt_frame);
        pde.set_present(true);
        pde.set_writable(true);
        pde.set_user_allowed(true);

    }
    ASSERT(pde.is_present());

    VirtualAddress pt_addr = pde.addr();
    if(tempMap_pageTable) {
        pt_addr = temp_map(pt_addr); // map page table so we can access it
    }

    if(new_pagetable) {
        // zero the new page table
        memset((void*)(u32)pt_addr, 0, PAGE_SIZE);
    }

    auto page_table = PageTable(pt_addr);

    auto pte = page_table.get_pte(addr);
    return pte;

}

void MemoryManager::disable_page(Frame frame) {
    auto pte = ensure_pte(frame);
    pte.set_addr(frame);
    pte.set_present(false);
    pte.set_writable(false);
    pte.set_user_allowed(false);
    un_temp_map();
    flush_tlb(frame);
}


void MemoryManager::allocate(VirtualAddress virt_addr, PageWritable writable, UserAllowed user_allowed) {
    InterruptDisabler dis;
    #ifdef MM_DBG
    kprintf("MM: allocate: 0x%x\n", virt_addr);
    #endif
    // LOCKER(get_memory_manager_lock());
    auto pte = ensure_pte(virt_addr);
    ASSERT(!pte.is_present());
    Err err;
    Frame pt_frame = get_free_frame(err);
    ASSERT(!err);
    pte.set_addr(pt_frame);
    pte.set_present(true);
    pte.set_writable(writable==PageWritable::YES);
    pte.set_user_allowed(user_allowed==UserAllowed::YES);
    un_temp_map(); // 'ensure_pte' temp_mapped the page table of PTE
    flush_tlb(virt_addr);
}

void MemoryManager::deallocate(VirtualAddress virt_addr, bool free_page) {
    #ifdef MM_DBG
    kprintf("MM: deallocate: 0x%x\n", virt_addr);
    #endif
    // LOCKER(get_memory_manager_lock());
    InterruptDisabler dis();
    auto pte = ensure_pte(virt_addr);
    ASSERT(pte.is_present());
    if(free_page) {
        set_frame_available(pte.addr());
    }
    pte.zero();
    // TODO: check if entire page table is empty.
    // if it is, free the page table's frame and zero its PDE
    un_temp_map(); // 'ensure_pte' temp_mapped the page table of PTE
    flush_tlb(virt_addr);

}

void MemoryManager::copy_from_physical_frame(PhysicalAddress src, u8* dst) {
    auto src_vaddr = temp_map(src);
    memcpy(dst, (u8*)(u32)src_vaddr, PAGE_SIZE);
    un_temp_map();
}

void MemoryManager::copy_to_physical_frame(PhysicalAddress dst, u8* src) {
    auto dst_vaddr = temp_map(dst);
    memcpy((u8*)(u32)dst_vaddr, src, PAGE_SIZE);
    un_temp_map();
}

void MemoryManager::memcpy_frames(PhysicalAddress dst, PhysicalAddress src) {
    // temp buffer on the heap to copy to/from frames
    // we need to use it since we only temp_map a single frame at a time
    // so we can't copy directly between two frames
    u8* temp_buff = new u8[PAGE_SIZE]; // allocate on heap because kernel stack is small
    copy_from_physical_frame(src, temp_buff);
    copy_to_physical_frame(dst, temp_buff);
    delete[] temp_buff;

}

#define DBG_CLONE_PAGE_DIRECTORY

/**
 * returns a new page directory that's cloned from this one
 * kernel-space PDEs are shallow copied - they point to the same page tables
 *  (this way changes in kernel sapce memory are synced across all tasks)
 * user-space PDEs are deep-cloned only if CopyUserPages is YES,
 *  otherwise, they are not copied over
 *   this flag should be YES for stuff like fork(), creating a new thread, etc
 * 
 * NOTE: if we create a new PDE in kernel-space,
 * it will not be synced across all kernel tasks!
 * that's why we have m_kernel_PDEs_locked
 */
PageDirectory MemoryManager::clone_page_directory(CopyUserPages copy_user_pages) {
    #ifdef DBG_CLONE_PAGE_DIRECTORY
    kprintf("MemoryManager::clone_page_directory from: 0x%x\n", (u32)m_page_directory->get_base());
    #endif
    // LOCKER(get_memory_manager_lock());
    InterruptDisabler dis();

    Err err;
    auto new_PD_addr = next_page_directory_allocation;
    next_page_directory_allocation += PAGE_SIZE;
    auto new_page_directory = PageDirectory(PhysicalAddress(new_PD_addr));

    // shallow copy of page directory
    memcpy_frames(new_page_directory.get_base(), m_page_directory->get_base());

    u32* PD_entries = new u32[NUM_PAGE_DIRECTORY_ENTRIES];
    copy_from_physical_frame((u32)m_page_directory->entries(), (u8*)PD_entries);

    u32* new_page_tables_addresses = new u32[NUM_PAGE_DIRECTORY_ENTRIES]; // allocate on heap because kernel stack is small
    memset(new_page_tables_addresses, 0, NUM_PAGE_DIRECTORY_ENTRIES*sizeof(u32));
    
    // used for userpages if CopyUserPages is 'NO'
    bool* delete_page_directory_entry = new bool[NUM_PAGE_DIRECTORY_ENTRIES];
    for(size_t i = 0; i < NUM_PAGE_DIRECTORY_ENTRIES; i++)
        delete_page_directory_entry[i] = false;

    // two passess over original page directory
    // first pass: 
    //            - if PDE points to user space: alocate & copy new page tables for each present PDE
    //            - otherwise, point to the same PT (don't copy page table) 
    // seconds pass: update PDEs in new page directory to point to the new page tables

    for(size_t pde_idx = 0; pde_idx < NUM_PAGE_DIRECTORY_ENTRIES; ++pde_idx) {
        // If this PDE entry is identity mapped,
        // skip it - new page directory will point to the same page table
        // it's OK since the identity map page tables
        // are not expected to change
        if(pde_idx <= IDENTITY_MAP_END/(4*MB)) {
            continue;
        }
        auto pde = PDE(PD_entries[pde_idx]);
        if(!pde.is_present())
            continue;
        auto page_table_addr = pde.addr();
        // don't copy page tables of kernel space
        // (kernel memory is shared among all tasks)
        if(address_in_kernel_space(pde_idx * PDE_MAP_SIZE)) {
            new_page_tables_addresses[pde_idx] = page_table_addr;
            continue;
        }
        if(copy_user_pages == CopyUserPages::NO) {
            delete_page_directory_entry[pde_idx] = true;
            continue;
        }
        #ifdef DBG_CLONE_PAGE_DIRECTORY
        kprintf("MemoryManager::clone page table: 0x%x\n", (u32)page_table_addr);
        #endif

        auto page_table = PageTable(page_table_addr);

        auto new_PT_addr = get_free_frame(err);
        ASSERT(!err);
        new_page_tables_addresses[pde_idx] = new_PT_addr;
        // kprintf("new page table addr: 0x%x\n", new_PT_addr);
        auto new_page_table = PageTable(new_PT_addr);
        // shallow copy of page table
        memcpy_frames(new_page_table.get_base(), page_table.get_base());
    }
    delete[] PD_entries;
    // point new PDEs to new page tables
    auto new_PD_vaddr = temp_map(new_page_directory.get_base());
    for(size_t pde_idx = 0; pde_idx < NUM_PAGE_DIRECTORY_ENTRIES; ++pde_idx) {
        auto new_pde = PDE(((u32*)new_PD_vaddr)[pde_idx]);
        if(delete_page_directory_entry[pde_idx]) {
            new_pde.zero();
            continue;
        }
        if(new_page_tables_addresses[pde_idx] == 0)
            continue;
        new_pde.set_addr(new_page_tables_addresses[pde_idx]);
    }
    un_temp_map();
    delete[] new_page_tables_addresses;
    delete[] delete_page_directory_entry;
    return new_page_directory;
}


MemoryManager& MemoryManager::the(u32 cr3) {
    ASSERT(mm != nullptr);
    // LOCKER(get_memory_manager_lock());
    InterruptDisabler dis();
    // update page table address to CR3 value
    if(cr3 == 0) {
        cr3 = get_cr3();
    }
    mm->m_page_directory->set_page_directoy_addr(PhysicalAddress(cr3));
    return *mm;
}

MemoryManager::MemoryManager()
    :  m_page_directory(nullptr),
     m_tempmap_used(false),
      m_kernel_PDEs_locked(false)
{
    memset(m_frames_avail_bitmap, 0, sizeof(m_frames_avail_bitmap));
    m_tempmap_used = false;
}

MemoryManager::~MemoryManager() {
    ASSERT(false);
}

bool address_in_user_space(VirtualAddress addr) {
    return addr >= USERSPACE_START && addr < KERNELSPACE_START;

}
bool address_in_kernel_space(VirtualAddress addr) {
    return addr < USERSPACE_START || addr >= KERNELSPACE_START;
}