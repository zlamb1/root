#ifndef ROOT_PSF_H
#define ROOT_PSF_H

#include "font.h"
#include "types.h"

root_err_t root_psf_get_font (root_font_t *font);

void *root_psf_get_glyph (const root_font_t *font, root_uint8_t glyph);

void root_psf_put_glyph (const root_font_t *font, root_uint8_t *dst,
                         root_uint8_t glyph, root_uint32_t fg,
                         root_uint32_t bg, const root_gfx_mode_t *gfx_mode);

#endif
