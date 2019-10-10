#pragma once

#include "types.h"
#include "MM/MM_types.h"

#define NUM_PAGE_DIRECTORY_ENTRIES 1024

class PageDirectory {

public:
    PageDirectory(PhysicalAddress addr): m_addr(addr) {
        addr.assert_aligned();
    }

    PTE ensure_pte(VirtualAddress addr);
    static u32 get_cr3();

private:
    PDE get_pde(VirtualAddress addr);

    PhysicalAddress m_addr;
    
};