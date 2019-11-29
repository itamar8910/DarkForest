#pragma once

#include "types/String.h"
#include "types/vector.h"

namespace Backtrace {
void print_backtrace(u32 eip, u32 ebp);

class SymbolsMap {

public:
    struct SymbolAndAddress {
        String symbol_name;
        u32 address;
        bool operator==(const SymbolAndAddress&) const;
    };
    void from_file(const String& path);

    const SymbolAndAddress* find(u32 address) const;

private:
    Vector<SymbolAndAddress> m_symbols;
};

class LinesMap {
public:
    struct SourceAndAddress {
        String file_name;
        u32 line_num;
        u32 address;
        bool operator==(const SourceAndAddress&) const;
    };
    void from_file(const String& path);
    SourceAndAddress* find(u32 address);
    Vector<SourceAndAddress>& lines() {return m_lines;}
private:
    Vector<SourceAndAddress> m_lines;
};

};