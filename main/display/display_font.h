#pragma once

#include <stdint.h>

#define DISPLAY_FONT_HEIGHT     28
#define DISPLAY_FONT_CELL_WIDTH 24
#define DISPLAY_FONT_ROW_BYTES  3
#define DISPLAY_FONT_BITMAP_BYTES (DISPLAY_FONT_HEIGHT * DISPLAY_FONT_ROW_BYTES)

typedef struct {
    uint32_t codepoint;
    uint8_t advance;
    uint8_t bitmap[DISPLAY_FONT_BITMAP_BYTES];
} display_font_glyph_t;

const display_font_glyph_t *display_font_find(uint32_t codepoint);
