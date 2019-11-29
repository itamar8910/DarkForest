#pragma once

#include "backtrace.h"

class KernelSymbols {
public:
    static KernelSymbols& the();
    static void initialize();

    const Backtrace::LinesMap& lines_map() const {return m_lines_map;}
    const Backtrace::SymbolsMap& symbols_map() const {return m_symbols_map;}

private:
    KernelSymbols() {};

    Backtrace::LinesMap m_lines_map;
    Backtrace::SymbolsMap m_symbols_map;
};