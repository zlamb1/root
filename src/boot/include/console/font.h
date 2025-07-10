#ifndef ROOT_FONT_H
#define ROOT_FONT_H 1

#include "gfx_mode.h"
#include "types.h"

typedef struct root_font_t
{
  root_uint8_t glyph_width, glyph_height;
  root_size_t glyph_stride;
  root_uint8_t *glyphs;
  unsigned *uc;
  void (*put_glyph) (const struct root_font_t *font, root_uint8_t *dst,
                     root_uint8_t glyph, root_uint32_t fg, root_uint32_t bg,
                     const root_gfx_mode_t *gfx_mode);
} root_font_t;

#endif
