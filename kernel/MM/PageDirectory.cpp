#include "PageDirectory.h"
#include "MM/MM_types.h"
#include "assert.h"

PDE PageDirectory::get_pde(VirtualAddress addr) {
    u32 pd_index = ((u32)addr) >> 22;
    ASSERT(pd_index < NUM_PAGE_DIRECTORY_ENTRIES, "index < num PD entries");
    return ((u64*)(u32)m_addr)[pd_index];
}

PTE PageDirectory::ensure_pte(VirtualAddress addr) {
    auto pde = get_pde(addr);
    if(!pde.Present) {
        // TODO: allocae a new PT
        // allocate a new frame (=PT)
        // map it with tempmap
        // add PTE in PT to point to virt_addr

    }
    // TODO cont.

}

u32 PageDirectory::get_cr3() {
    u32 val;
    asm("movl %%cr3, %%eax" : "=a"(val));
    return val;
}
