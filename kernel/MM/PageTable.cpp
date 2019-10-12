#include "PageTable.h"

PTE PageTable::get_pte(VirtualAddress addr) {
    u32 pt_index = get_index(addr);
    ASSERT(pt_index < NUM_PAGE_Table_ENTRIES, "index < num PT entries");
    return PTE(entries()[pt_index]);
}

u32* PageTable::entries() {
    return (u32*)((u32)m_addr);
}

u32 PageTable::get_index(VirtualAddress addr) {
    return (((u32)addr) >>12) & 0x3ff;
}