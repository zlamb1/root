#ifndef ROOT_PSF2_H
#define ROOT_PSF2_H

#include "font.h"
#include "types.h"

root_font_t root_ps2f_get_font (void);

void *root_psf2_get_glyph (const root_font_t *font, root_u8 glyph);

void root_ps2f_put_glyph (const root_font_t *font, root_u8 *dst, root_u8 glyph,
                          root_u32 fg, root_u32 bg,
                          const root_gfx_mode_t *gfx_mode);

#endif
