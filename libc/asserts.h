#pragma once

#define ASSERTS_LEVEL_1

#ifdef __cplusplus
extern "C" {
#endif

void assertion_failed (const char* expression, const char* file, int line, const char* func) __attribute__ ((noreturn));


#define ASSERT(e) ((e)?(void)0:assertion_failed(#  e, __FILE__, __LINE__, __FUNCTION__))
#define ASSERT_NOT_REACHED() (assertion_failed("assert not reached", __FILE__, __LINE__, __FUNCTION__))
#define NOT_IMPLEMENTED() (assertion_failed("not implemented", __FILE__, __LINE__, __FUNCTION__))

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED();
#endif

#ifdef __cplusplus
}
#endif
