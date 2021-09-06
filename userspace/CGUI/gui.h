#pragma once

#include "types.h"

/// C Bindings for GUI ///
#ifdef __cplusplus
extern "C" {
#endif


typedef void* DF_WINDOW;

DF_WINDOW cgui_create_window(uint16_t width, uint16_t height);

int cgui_has_pending_message();

void cgui_draw_window(DF_WINDOW window, uint32_t* buffer);

#ifdef __cplusplus
}
#endif
