#pragma once

#include "types.h"
#include "bits.h"
#include "assert.h"

#define MULTIBOOT_MEMORY_AVAILABLE 1

constexpr u32 MAX_MEMORY_SIZE = UINT32_MAX;
constexpr u32 PAGE_SIZE = 4 * KB;
constexpr u32 N_FRAMES = MAX_MEMORY_SIZE / PAGE_SIZE;
constexpr u32 N_FRAME_BITMAP_ENTRIES = N_FRAMES / 32;

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

/**
 * Page Directory Entry
 */
struct PDE {
public:
	enum Flags {
		User = 1<<2, // can user access ?
		Write = 1<<3, // is writable?
		Present = 1<<0, // is present?
	};
	PDE(u64 entry) :
		m_PT_addr((u32)(entry>>22)) ,
		m_user(get_bit((u32)(entry&0xfff), User)){
			
	}
private:


	PhysicalAddress m_PT_addr; // page table address
	bool m_user;
	bool m_write;
	bool m_present;
	
};

/**
 * Page Table Entry
 */
struct PTE {
private:
	Frame frame;
	bool user;
	bool write;
	bool present;
	
};