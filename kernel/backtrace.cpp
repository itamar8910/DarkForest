#include "backtrace.h"

#include "logging.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/VFS.h"
#include "stdlib.h"
#include "kernel_symbols.h"

using namespace Backtrace;


void Backtrace::print_backtrace(u32 eip, u32 ebp) {
   kprintf("backtrace: eip:0x%x, ebp:0x%x\n", eip, ebp);
   auto* source_info = KernelSymbols::the().lines_map().find(eip);
   ASSERT(source_info != nullptr, "print_backtrace: couldn't get source info");
   kprintf("%s:%d  0x%x\n", source_info->file_name.c_str(), source_info->line_num+1, source_info->address);
   for(u32* stack = (u32*) ebp; stack[0] != 0; stack = (u32*) stack[0]) {
      u32 addr_in_next_function = stack[1];
      kprintf("0x%x\n", addr_in_next_function);
   }
}

void LinesMap::from_file(const String& path) {
   CharFile* f = static_cast<CharFile*>(VFS::the().open(path));
   ASSERT(f != nullptr, "LinesMap::from_file can't open file");
   char* content_raw = f->get_content();
   kprintf("aftet get content\n");
   String content(content_raw);
   // kprintf("content: %s\n", content.c_str());
   auto lines = content.split('\n', DEFAULT_LINES_CAPACITY);
   ASSERT(lines.size() > 0, "LinesMap::from_file, lines file is empty");
   kprintf("## lines: %d\n", lines.size());
   for(auto& line : lines) {
      auto vals = line.split(' ');
      ASSERT(vals.size() == 3, "size==3");
      SourceAndAddress entry{
         vals[1],
         static_cast<u32>(atoi(vals[2].c_str())),
         static_cast<u32>(atoi(vals[0].c_str(), 16)),
      };
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

template <typename T>
const T* find_containing_address(const Vector<T>& vec, u32 address)
{
   // TODO: do a binsearch
   for(size_t i = 0; i < vec.size()-1; i++) {
      if(address < vec[i].address) {
         break;
      }
      if(
         address < vec[i+1].address 
      ) {
         return &(vec[i]);
      }
   }
   return nullptr;
}

const LinesMap::SourceAndAddress* LinesMap::find(u32 address) const {
   return find_containing_address(m_lines, address);
}