#pragma once

#include "types.h"
#include "types/String.h"

void load_and_jump_userspace(const String& path);
void load_and_jump_userspace(void* elf_data, u32 size);

extern "C" void jump_to_usermode(void (*func)(), u32 user_esp);