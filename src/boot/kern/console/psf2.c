#include "console/psf2.h"
#include "console/font.h"

extern struct root_psf2_header_t *_binary_font_Lat2_Terminus20x10_psf_start[];
extern unsigned char *_binary_font_Lat2_Terminus20x10_psf_end[];
extern int _binary_font_Lat2_Terminus20x10_psf_size[];

typedef struct root_psf2_header_t
{
  unsigned char sig[4];
  root_u32 version;
  root_u32 header_size;
  root_u32 flags;
  root_u32 num_glyphs;
  root_u32 bytes_per_glyph;
  root_u32 glyph_height;
  root_u32 glyph_width;
} __attribute__ ((packed)) root_psf2_header_t;

root_font_t
root_ps2f_get_font (void)
{
  root_psf2_header_t *header
      = (root_psf2_header_t *) _binary_font_Lat2_Terminus20x10_psf_start;
  return (root_font_t){ .glyph_width = header->glyph_width,
                        .glyph_height = header->glyph_height,
                        .glyph_stride = header->bytes_per_glyph,
                        .glyphs = (root_u8 *) (header + 1),
                        .put_glyph = root_ps2f_put_glyph };
}

void *
root_psf2_get_glyph (const root_font_t *font, root_u8 glyph)
{
  return font->glyphs + glyph * font->glyph_stride;
}

void
root_ps2f_put_glyph (const root_font_t *font, root_u8 *dst, root_u8 glyph,
                     root_u32 fg, root_u32 bg, const root_gfx_mode_t *gfx_mode)
{
  root_size_t bytes_per_row = (font->glyph_width + 7) >> 3;
  root_u8 *glyph_data = root_psf2_get_glyph (font, glyph);
  for (root_u16 y = 0; y < font->glyph_height; y++)
    {
      for (root_u16 x = 0; x < font->glyph_width; x++)
        {
          root_size_t i = y * gfx_mode->stride + x * gfx_mode->bpp;
          root_u32 color = bg;
          if (glyph_data[x >> 3] << (x % 8) & 0x80)
            color = fg;
          dst[i] = color & 0xFF;
          dst[i + 1] = color >> 8 & 0xFF;
          dst[i + 2] = color >> 16 & 0xFF;
          dst[i + 3] = color >> 24 & 0xFF;
        }
      glyph_data += bytes_per_row;
    }
}
