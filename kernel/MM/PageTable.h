
#pragma once

#include "types.h"
#include "MM/MM_types.h"

#define NUM_PAGE_Table_ENTRIES 1024

class PageTable {

public:
    PageTable(VirtualAddress addr): m_addr(addr) {}

    PTE get_pte(VirtualAddress addr);

    static u32 get_index(VirtualAddress);

private:

    u32* entries();
    VirtualAddress m_addr;
    
};