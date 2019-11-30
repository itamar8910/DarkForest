#pragma once

#ifndef H_ASSERTS
#define H_ASSERTS
#define ASSERTS_LEVEL_1

void ASSERT(bool x, const char* msg = "[Unnamed]");
void NOT_IMPLEMENTED [[noreturn]] (const char* msg);
void ASSERT_NOT_REACHED [[noreturn]] (const char* msg);

void assertion_failed(const char* expression, const char* msg, const char* file, int line, const char* func);
#define XASSERT(e) ((e)?(void)0:assertion_failed(#  e, "", __FILE__, __LINE__, __FUNCTION__))
// #define XASSERT(e, msg) ((e)?0:assertion_failed(#  e, (msg), __FILE__, __LINE__))

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED();
#endif

#endif