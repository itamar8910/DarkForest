#include "backtrace.h"

#include "logging.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/VFS.h"
#include "stdlib.h"

using namespace Backtrace;

void Backtrace::print_backtrace(u32 eip, u32 ebp) {
   kprintf("backtrace: eip:0x%x, ebp:0x%x\n", eip, ebp);
   for(u32* stack = (u32*) ebp; stack[0] != 0; stack = (u32*) stack[0]) {
      u32 addr_in_next_function = stack[1];
      kprintf("0x%x\n", addr_in_next_function);
   }
}

void LinesMap::from_file(const String& path) {
   CharFile* f = static_cast<CharFile*>(VFS::the().open(path));
   ASSERT(f != nullptr, "LinesMap::from_file can't open file");
   char* content_raw = f->get_content();
   String content(content_raw);
   kprintf("content: %s\n", content.c_str());
   auto lines = content.split('\n');
   kprintf("## lines: %d\n", lines.size());
   for(auto& line : lines) {
      auto vals = line.split(' ');
      ASSERT(vals.size() == 3);
      SourceAndAddress entry{
         vals[1],
         static_cast<u32>(atoi(vals[2].c_str())),
         static_cast<u32>(atoi(vals[0].c_str(), 16)),
      };
      kprintf("append\n");
      m_lines.append(entry);
   }
   delete[] content_raw;
}

void SymbolsMap::from_file(const String& path) {
   (void)path;
   NOT_IMPLEMENTED("SymbolsMap::from_file");
}

bool SymbolsMap::SymbolAndAddress::operator==(const SymbolsMap::SymbolAndAddress& other) const {
   return symbol_name==other.symbol_name && address == other.address;
}

bool LinesMap::SourceAndAddress::operator==(const LinesMap::SourceAndAddress& other) const {
   return file_name == other.file_name 
          && line_num == other.line_num 
          && address == other.address;
}