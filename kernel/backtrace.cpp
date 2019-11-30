#include "backtrace.h"

#include "logging.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/VFS.h"
#include "stdlib.h"
#include "kernel_symbols.h"

using namespace Backtrace;


void Backtrace::print_backtrace(u32 eip, u32 ebp) {
   // kprintf("backtrace: eip:0x%x, ebp:0x%x\n", eip, ebp);
   auto* source_info = KernelSymbols::the().lines_map().locate(eip);
   ASSERT(source_info != nullptr);
   kprint("Generated from:\n");
   kprintf(">> 0x%x %s:%d\n", source_info->address, source_info->file_name.c_str(), source_info->line_num+1);
   kprint("Backtrace:\n");
   auto* symbol_info = KernelSymbols::the().symbols_map().locate(eip);
   ASSERT(symbol_info != nullptr);
   kprintf(">> 0x%x %s\n", symbol_info->address, symbol_info->symbol_name.c_str());
   for(u32* stack = (u32*) ebp; stack[0] != 0; stack = (u32*) stack[0]) {
      u32 addr_in_next_function = stack[1];
      // kprintf("0x%x\n", addr_in_next_function);
      auto* symbol_info = KernelSymbols::the().symbols_map().locate(addr_in_next_function);
      kprintf(">> 0x%x %s\n", symbol_info->address, symbol_info->symbol_name.c_str());
   }
}