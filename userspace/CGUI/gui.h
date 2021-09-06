#pragma once

#include "types.h"

/// C Bindings for GUI ///
#ifdef __cplusplus
extern "C" {
#endif


typedef void* DF_WINDOW;

DF_WINDOW cgui_create_window(uint16_t width, uint16_t height);


void cgui_draw_window(DF_WINDOW window, uint32_t* buffer);

typedef struct __attribute__((packed))
{
    int pressed;
    uint8_t key;
} CGUI_KeyEvent ;

int cgui_get_key_event(CGUI_KeyEvent* out_event);

#ifdef __cplusplus
}
#endif
