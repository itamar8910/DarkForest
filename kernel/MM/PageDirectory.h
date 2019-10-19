#pragma once

#include "types.h"
#include "MM/MM_types.h"

#define NUM_PAGE_DIRECTORY_ENTRIES 1024

class PageDirectory {

public:
    PageDirectory(PhysicalAddress addr): m_addr(addr) {
        addr.assert_aligned();
    }

    PDE get_pde(VirtualAddress addr);
    static u32 get_index(VirtualAddress);
    void set_page_directoy_addr(PhysicalAddress addr) {
        addr.assert_aligned();
        m_addr = addr;
    };
    u32* entries();
    u32 inline get_entry(int idx) const;
    PhysicalAddress get_base() {return m_addr;}


private:
    PhysicalAddress m_addr;
    
};