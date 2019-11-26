#pragma once

#define ASSERTS_LEVEL_1

void ASSERT(bool x, const char* msg = "[Unnamed]");
void NOT_IMPLEMENTED [[noreturn]] (const char* msg);
void ASSERT_NOT_REACHED [[noreturn]] (const char* msg);
#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED();
#endif