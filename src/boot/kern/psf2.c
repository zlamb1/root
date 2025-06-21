#include "psf2.h"

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

unsigned char *
root_psf2_getglyph (root_u8 ch)
{
  root_psf2_header_t *header
      = (root_psf2_header_t *) _binary_font_Lat2_Terminus20x10_psf_start;
  unsigned char *data = (unsigned char *) (header + 1);
  return data + ch * header->bytes_per_glyph;
}

void
root_psf2_putglyph (void *data, root_u8 ch, root_u32 fg, root_u32 bg,
                    root_size_t pitch)
{
  unsigned char *glyph = root_psf2_getglyph (ch);
  unsigned char *dst = (unsigned char *) data;
  for (int y = 0; y < 20; y++)
    {
      for (int x = 0; x < 10; x++)
        {
          root_size_t idx = y * pitch + x * 4;
          root_u32 color = bg;
          if (glyph[x / 8] << (x % 8) & 0x80)
            color = fg;
          dst[idx] = color & 0xFF;
          dst[idx + 1] = color >> 8 & 0xFF;
          dst[idx + 2] = color >> 16 & 0xFF;
          dst[idx + 3] = color >> 24 & 0xFF;
          (void) color;
        }
      glyph += 2;
    }
}
