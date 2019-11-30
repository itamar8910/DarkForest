#pragma once

#ifndef H_ASSERTS
#define H_ASSERTS
#define ASSERTS_LEVEL_1

// void ASSERT(bool x);
void NOT_IMPLEMENTED [[noreturn]] (const char* msg);
void ASSERT_NOT_REACHED [[noreturn]] (const char* msg);

void assertion_failed(const char* expression, const char* file, int line, const char* func);
#define ASSERT(e) ((e)?(void)0:assertion_failed(#  e, __FILE__, __LINE__, __FUNCTION__))
// #define XASSERT(e)

#ifdef KERNEL
void ASSERT_INTERRUPTS_DISABLED();
#endif

#endif