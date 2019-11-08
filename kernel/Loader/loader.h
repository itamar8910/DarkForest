#pragma once

#include "types.h"

void load_and_jump_userspace(void* elf_data, u32 size);

extern "C" void jump_to_usermode(void (*func)(), u32 user_esp);