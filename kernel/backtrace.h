#pragma once

#include "types/String.h"
#include "types/vector.h"
#include "FileSystem/CharFile.h"
#include "FileSystem/VFS.h"

namespace Backtrace {
void print_backtrace(u32 eip, u32 ebp);


// TODO: templatize SymbolsMap, LinesMap

template<typename T>
class AddressMap {
public:
    AddressMap(size_t init_capacity=0)
    : m_entries(init_capacity)
    {
    }
    virtual void from_file(const String& path) = 0;
    const T* locate(u32 address) const;
protected:
    Vector<T> m_entries;
};

struct SymbolAndAddress {
    String symbol_name;
    u32 address;
    bool operator==(const SymbolAndAddress& other) const {
        return symbol_name==other.symbol_name && address == other.address;
    }
};

struct SourceAndAddress {
    String file_name;
    u32 line_num;
    u32 address;
    bool operator==(const SourceAndAddress& other) const {
        return file_name == other.file_name 
                && line_num == other.line_num 
                && address == other.address;
    }
};

constexpr size_t DEFAULT_NUM_LINE_MAPS = 10000;
constexpr size_t DEFAULT_NUM_SYMB_MAPS = 1000;

class SymbolsMap : public AddressMap<SymbolAndAddress> {
public:
    SymbolsMap() : AddressMap<SymbolAndAddress>(DEFAULT_NUM_SYMB_MAPS){};
    virtual void from_file(const String& path) override {
        CharFile* f = static_cast<CharFile*>(VFS::the().open(Path(path)));
        ASSERT(f != nullptr);
        char* content_raw = f->get_content();
        kprintf("aftet get content\n");
        String content(content_raw);
        // kprintf("content: %s\n", content.c_str());
        auto lines = content.split('\n', DEFAULT_NUM_LINE_MAPS);
        ASSERT(lines.size() > 0);
        kprintf("## lines: %d\n", lines.size());
        for(auto& line : lines) {
            ASSERT(line.find(' ') != -1);
            auto addr_str = line.substr(0, line.find(' '));
            auto symbol_str = line.substr(line.find(' ') + 1);
            SymbolAndAddress entry{
                symbol_str,
                static_cast<u32>(df_atoi(addr_str.c_str(), 16)),
            };
            m_entries.append(entry);
        }
        delete[] content_raw;
    }
};

class LinesMap : public AddressMap<SourceAndAddress> {
public:
    LinesMap() : AddressMap<SourceAndAddress>(DEFAULT_NUM_LINE_MAPS){};
    virtual void from_file(const String& path) override {
        CharFile* f = static_cast<CharFile*>(VFS::the().open(Path(path)));
        ASSERT(f != nullptr);
        char* content_raw = f->get_content();
        kprintf("aftet get content\n");
        String content(content_raw);
        // kprintf("content: %s\n", content.c_str());
        auto lines = content.split('\n', DEFAULT_NUM_LINE_MAPS);
        ASSERT(lines.size() > 0);
        kprintf("## lines: %d\n", lines.size());
        for(auto& line : lines) {
            auto vals = line.split(' ');
            ASSERT(vals.size() == 3);
            SourceAndAddress entry{
                vals[1],
                static_cast<u32>(df_atoi(vals[2].c_str(), 10)),
                static_cast<u32>(df_atoi(vals[0].c_str(), 16)),
            };
            m_entries.append(entry);
        }
        delete[] content_raw;
    }
};

template <typename T>
const T* AddressMap<T>::locate(u32 address) const
{
   // TODO: do a binsearch
   for(size_t i = 0; i < m_entries.size()-1; i++) {
      if(address < m_entries[i].address) {
         break;
      }
      if(
         address < m_entries[i+1].address 
      ) {
         return &(m_entries[i]);
      }
   }
   return nullptr;
}

}
