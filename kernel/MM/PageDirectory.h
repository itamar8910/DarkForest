#pragma once

#include "types.h"
#include "MM/MM_types.h"

#define NUM_PAGE_DIRECTORY_ENTRIES 1024

class PageDirectory {

public:
    PageDirectory(PhysicalAddress addr): m_addr(addr) {
        addr.assert_aligned();
    }

    static u32 get_cr3();
    PDE get_pde(VirtualAddress addr);
    static u32 get_index(VirtualAddress);

private:
    u32* entries();

    PhysicalAddress m_addr;
    
};