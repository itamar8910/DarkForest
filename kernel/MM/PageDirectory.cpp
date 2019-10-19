#include "PageDirectory.h"
#include "MM/MM_types.h"
#include "Kassert.h"
#include "PageTable.h"

PDE PageDirectory::get_pde(VirtualAddress addr) {
    u32 pd_index = get_index(addr);
    ASSERT(pd_index < NUM_PAGE_DIRECTORY_ENTRIES, "index < num PD entries");
    return PDE(entries()[pd_index]);
}



u32* PageDirectory::entries() {
    return (u32*)((u32)m_addr);
}

u32 PageDirectory::get_index(VirtualAddress addr) {
    return (((u32)addr) >> 22) & 0x3ff;
}

u32 inline PageDirectory::get_entry(int idx) const {
    return ((u32*)((u32)m_addr))[idx];
}

// void PageDirectory::copy_entries_from( const PageDirectory& other) {
//     auto my_entries = this->entries();
//     for(size_t pde_idx = 0; pde_idx < NUM_PAGE_DIRECTORY_ENTRIES; ++pde_idx) {
//         my_entries[pde_idx] = other.get_entry(pde_idx);
//     }
// }
