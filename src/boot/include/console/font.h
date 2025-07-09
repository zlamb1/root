#ifndef ROOT_FONT_H
#define ROOT_FONT_H 1

#include "gfx_mode.h"
#include "types.h"

typedef struct root_font_t
{
  root_u8 glyph_width, glyph_height;
  root_size_t glyph_stride;
  root_u8 *glyphs;
  unsigned *uc;
  void (*put_glyph) (const struct root_font_t *font, root_u8 *dst,
                     root_u8 glyph, root_u32 fg, root_u32 bg,
                     const root_gfx_mode_t *gfx_mode);
} root_font_t;

#endif
