
#include "logging.h"
#include "DebugPort.h"

void kprint(const char* str) {
    DebugPort::write(str);
}