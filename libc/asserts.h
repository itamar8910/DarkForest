#pragma once

void ASSERT(bool x, const char* msg = "[Unnamed]");
void NOT_IMPLEMENTED(const char* msg);
void ASSERT_NOT_REACHED(const char* msg);
#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED();
#endif