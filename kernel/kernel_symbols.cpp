#include "kernel_symbols.h"
#include "logging.h"

constexpr const char* kernel_lines_file = "/initrd/kernel_lineinfo.txt";
constexpr const char* kernel_symbols = "/initrd/kernel_symbols.txt";

static KernelSymbols* s_the = nullptr;

KernelSymbols& KernelSymbols::the() {
    if(!s_the) {
        KernelSymbols::initialize();
    }
    return *s_the;
}

void KernelSymbols::initialize() {
    s_the = new KernelSymbols();
    s_the->m_lines_map.from_file(kernel_lines_file);
    // s_the->m_symbols_map.from_file(kernel_symbols);
    kprintf("# lines: %d\n", s_the->m_lines_map.lines().size());
    for(auto& l : s_the->m_lines_map.lines()) {
        kprintf("0x%x: %s, %d\n", l.address, l.file_name.c_str(), l.line_num);
    }
}