#pragma once


#include "multiboot.h"
#include "types.h"
#include "stdlib.h"
#include "bits.h"
#include "Kassert.h"
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

constexpr u32 TEMPMAP_ADDR = (u32)((4*MB) - PAGE_SIZE);



class MemoryManager {
private:
	MemoryManager(); // singelton
	~MemoryManager();
    void init(multiboot_info_t* mbt);


public:
    static void initialize(multiboot_info_t* mbt);
	static MemoryManager& the();

	VirtualAddress temp_map(PhysicalAddress addr);
	void allocate(VirtualAddress virt_addr, bool writable, bool user_allowed);
	void flush_tlb(VirtualAddress addr);
    PTE ensure_pte(VirtualAddress addr, bool create_new);
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