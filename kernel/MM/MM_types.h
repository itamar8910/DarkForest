#pragma once

#include "types.h"
#include "bits.h"
#include "Kassert.h"

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
		PhysicalAddress(u32 addr) : m_addr(addr) {}
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

enum PageFlags {
	User = 2, // can user access ?
	Write = 3, // is writable?
	Present = 0, // is present?
};

struct TableEntry {
public:
	TableEntry(u32& entry) : m_entry(entry) {}

	u16 flags() const {
		return m_entry & 0xfff;
	}
	PhysicalAddress addr() const {
		return m_entry & ~0xfff; // clear flags
	}

	void set_addr(PhysicalAddress addr) {
		m_entry &= 0xfff; // clear addr
		m_entry |= addr << 12;
	}
	void set_flags(u16 flags) {
		m_entry &= ~0xfff; // clear addr
		m_entry |= flags;
	}

	bool is_present() {
		return flags() & (1<<PageFlags::Present);
	}
	bool is_writable() {
		return flags() & (1<<PageFlags::Write);
	}
	bool is_user_allowed() {
		return flags() & (1<<PageFlags::User);
	}
	void set_present(bool val) {
		u16 f = flags();
		set_bit(f, PageFlags::Present, val);
		set_flags(f);
	}
	void set_writable(bool val) {
		u16 f = flags();
		set_bit(f, PageFlags::Write, val);
		set_flags(f);
	}
	void set_user_allowed(bool val) {
		u16 f = flags();
		set_bit(f, PageFlags::User, val);
		set_flags(f);
	}

private:
	u32& m_entry;

};



struct PDE : public TableEntry {
public:
	PDE(u32& addr) : TableEntry(addr) {}

};
struct PTE : public TableEntry {
	PTE(u32& addr) : TableEntry(addr) {}
};


