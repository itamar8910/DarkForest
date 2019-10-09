#pragma once

#include "multiboot.h"
#include "types.h"
#include "stdlib.h"
#include "bits.h"
#include "assert.h"

/*
    Virtual Memory map:
    (0MB-4MB - identity mapped)
    3MB - 4MB: kmalloc
	4MB-PAGE_SIZE...4MB - TempMap page (for a way to access a physical address)
    0xc0000000 - 0xffffffff : kernel address space
*/


#define MULTIBOOT_MEMORY_AVAILABLE 1

constexpr u32 MAX_MEMORY_SIZE = UINT32_MAX;
constexpr u32 PAGE_SIZE = 4 * KB;
constexpr u32 N_FRAMES = MAX_MEMORY_SIZE / PAGE_SIZE;
constexpr u32 N_FRAME_BITMAP_ENTRIES = N_FRAMES / 32;

#define ERR_NO_FREE_FRAMES 1

struct BitmapEntry {
	u32 m_entry_idx;
	u32 m_entry_bit;
	BitmapEntry(u32 entry_idx, u32 entry_bit):
		m_entry_idx(entry_idx),
		m_entry_bit(entry_bit) {}
};

class PhysicalAddress {
	protected:
		u32 m_addr;
	public:
		explicit PhysicalAddress(u32 addr) : m_addr(addr) {}
		BitmapEntry get_bitmap_entry() const {
			u32 frame_idx = m_addr / PAGE_SIZE;
			return BitmapEntry(
				frame_idx / 32,
				frame_idx % 32
			);
		}
		bool is_frame_aligned() const {
			return m_addr % PAGE_SIZE == 0;
		}
		void assert_aligned() const {
			ASSERT(is_frame_aligned(), "frame not aligned");
		}
		operator int() const{return m_addr;} // cast to int
};

// A frame is a page-aligned physical address
class Frame: public PhysicalAddress {
	public:
		Frame(const PhysicalAddress& addr): PhysicalAddress(addr) {
			assert_aligned();
		}
		Frame(u32 addr) : PhysicalAddress(addr) {
			assert_aligned();
		}

		using PhysicalAddress::PhysicalAddress;

		static Frame from_bitmap_entry(const BitmapEntry& entry) {
			return Frame((entry.m_entry_idx * 32 * PAGE_SIZE) + entry.m_entry_bit*PAGE_SIZE);
		}
};

typedef u32 VirtualAddress;

struct [[gnu::packed]] MultibootMemMapEntry {
	unsigned int size;
	u64 base;
	u64 len;
	unsigned int type;
};

static_assert(sizeof(MultibootMemMapEntry)==24);


class MemoryManager {
private:
	MemoryManager(); // singelton
	~MemoryManager();
    void init(multiboot_info_t* mbt);


public:
    static void initialize(multiboot_info_t* mbt);
	static MemoryManager& the();

	Frame get_free_frame(Err&);
	void set_frame_used(const Frame& frame);
	int allocate(VirtualAddress virt_addr, int &err);

	void set_frame_available(Frame frame);
	bool is_frame_available(const Frame frame);

private:
	u32 m_frames_avail_bitmap[N_FRAME_BITMAP_ENTRIES]; // is the frame available for the OS? can it be accessed?
	// u32 m_frames_free_bitmap[N_FRAME_BITMAP_ENTRIES]; // is the frame currently free?

};