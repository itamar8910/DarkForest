#pragma once

#include "cpu.h"
#include "logging.h"

void ASSERT(bool x, const char* msg);
void NOT_IMPLEMENTED(const char* msg);
void ASSERT_NOT_REACHED(const char* msg);