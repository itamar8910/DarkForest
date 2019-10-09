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

struct BitmapEntry {
	u32 m_entry_idx;
	u32 m_entry_bit;
	BitmapEntry(u32 entry_idx, u32 entry_bit):
		m_entry_idx(entry_idx),
		m_entry_bit(entry_bit) {}
};

class PhysicalAddress {
	private:
		u32 m_addr;
	public:
		PhysicalAddress(u32 addr) : m_addr(addr) {}
		BitmapEntry get_bitmap_entry() {
			u32 frame_idx = m_addr / PAGE_SIZE;
			return BitmapEntry(
				frame_idx / 32,
				frame_idx % 32
			);
		}
		bool is_frame_aligned() {
			return m_addr % PAGE_SIZE == 0;
		}
		void assert_aligned() {
			ASSERT(is_frame_aligned(), "frame not aligned");
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
    void init(multiboot_info_t* mbt);

	MemoryManager();
	~MemoryManager();
public:
    static void initialize(multiboot_info_t* mbt);
	static MemoryManager& the();

	int allocate(VirtualAddress virt_addr);

	void set_frame_available(PhysicalAddress frame) {
		frame.assert_aligned();
		auto bitmap_entry = frame.get_bitmap_entry();
		set_bit(
			m_frames_avail_bitmap[bitmap_entry.m_entry_idx],
			bitmap_entry.m_entry_bit
		);
	}
	bool is_frame_available(PhysicalAddress frame) {
		frame.assert_aligned();
		auto bitmap_entry = frame.get_bitmap_entry();
		return get_bit(
			m_frames_avail_bitmap[bitmap_entry.m_entry_idx],
			bitmap_entry.m_entry_bit
		);

	}
private:
	u32 m_frames_avail_bitmap[N_FRAMES / 32]; // is the frame available for the OS? can it be accessed?
	u32 m_frames_free_bitmap[N_FRAMES / 32]; // is the frame currently free?

};