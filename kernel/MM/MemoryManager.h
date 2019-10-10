#pragma once


#include "multiboot.h"
#include "types.h"
#include "stdlib.h"
#include "bits.h"
#include "assert.h"
#include "MM/MM_types.h"
#include "types.h"
#include "PageDirectory.h"

/*
    Virtual Memory map:
    (0MB-4MB - identity mapped)
    3MB - 4MB: kmalloc
	4MB-PAGE_SIZE...4MB - TempMap page (for a way to access a physical address)
    0xc0000000 - 0xffffffff : kernel address space
*/



#define ERR_NO_FREE_FRAMES 1



class MemoryManager {
private:
	MemoryManager(); // singelton
	~MemoryManager();
    void init(multiboot_info_t* mbt);


public:
    static void initialize(multiboot_info_t* mbt);
	static MemoryManager& the();

	int allocate(VirtualAddress virt_addr);
	PDE get_pde(VirtualAddress virt_addr);
	PTE get_pte(VirtualAddress virt_addr, const PDE& pde);

	Frame get_free_frame(Err&);
	void set_frame_used(const Frame& frame);

	void set_frame_available(Frame frame);
	bool is_frame_available(const Frame frame);

private:
	u32 m_frames_avail_bitmap[N_FRAME_BITMAP_ENTRIES]; // is the frame available for the OS? can it be accessed?
	// u32 m_frames_free_bitmap[N_FRAME_BITMAP_ENTRIES]; // is the frame currently free?
	PageDirectory* m_page_directory; // TODO: make this a shared_ptr

};