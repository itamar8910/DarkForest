#pragma once

#include "multiboot.h"
#include "types.h"

struct [[gnu::packed]] MultibootMemMapEntry {
	unsigned int size;
	u64 base;
	u64 len;
	unsigned int type;
};

static_assert(sizeof(MultibootMemMapEntry)==24);

class MemoryManager {
public:
    static void initialize(multiboot_info_t* mbt);
};