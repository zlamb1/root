#include "console/psf.h"
#include "console/font.h"
#include "memory/malloc.h"
#include "string.h"
#include "types.h"

extern unsigned char *_binary_font_consolefont_psf_start[];
extern unsigned char *_binary_font_consolefont_psf_end[];
extern unsigned int _binary_font_consolefont_psf_size[];

#define PSF1_MODES512 0x1
#define PSF1_MODETAB  0x2
#define PSF1_MODESEQ  0x4

typedef struct
{
  unsigned char sig[2];
  unsigned char mode;
  unsigned char glyph_size;
} __attribute__ ((packed)) root_psf1_header_t;

typedef struct
{
  unsigned char sig[4];
  root_uint32_t version;
  root_uint32_t header_size;
  root_uint32_t flags;
  root_uint32_t num_glyphs;
  root_uint32_t bytes_per_glyph;
  root_uint32_t glyph_height;
  root_uint32_t glyph_width;
} __attribute__ ((packed)) root_psf2_header_t;

root_err_t
root_psf_get_font (root_font_t *font)
{
  unsigned char *data;
  unsigned size = (unsigned) _binary_font_consolefont_psf_size;

  if (font == NULL)
    return ROOT_EARG;
  if (size < 4)
    return ROOT_EINVAL;
  data = (unsigned char *) _binary_font_consolefont_psf_start;
  if (data[0] == 0x36 && data[1] == 0x04)
    {
      root_psf1_header_t *header
          = (root_psf1_header_t *) _binary_font_consolefont_psf_start;
      font->glyph_width = 8;
      font->glyph_height = header->glyph_size;
      font->glyph_stride = header->glyph_size;
      font->glyphs = (root_uint8_t *) (header + 1);
      font->put_glyph = root_psf_put_glyph;
      if (header->mode & PSF1_MODETAB || header->mode & PSF1_MODESEQ)
        {
          unsigned short *wdata;
          char ignore = 0;
          root_size_t glyph_index = 0;
          root_size_t num_glyphs = header->mode & PSF1_MODES512 ? 512 : 256;
          font->uc = root_malloc (num_glyphs);
          if (font->uc == NULL)
            return ROOT_EALLOC;
          root_memset (font->uc, 0, num_glyphs);
          wdata = (unsigned short *) _binary_font_consolefont_psf_start;
          wdata += (sizeof (root_psf1_header_t)
                    + header->glyph_size * num_glyphs)
                   >> 1;
          while (wdata < (unsigned short *) _binary_font_consolefont_psf_end)
            {
              if (wdata[0] == 0xFFFF)
                {
                  ignore = 0;
                  if (++glyph_index >= num_glyphs)
                    break;
                }
              else if (wdata[0] == 0xFFFE)
                /* ignore sequences of unicode code points */
                ignore = 1;
              else if (!ignore && wdata[0] < num_glyphs)
                font->uc[wdata[0]] = glyph_index;
              wdata++;
            }
        }
      else
        font->uc = NULL;
    }
  else if (data[0] == 0x72 && data[1] == 0xB5 && data[2] == 0x4A
           && data[3] == 0x86)
    {
      root_psf2_header_t *header
          = (root_psf2_header_t *) _binary_font_consolefont_psf_start;
      font->glyph_width = header->glyph_width;
      font->glyph_height = header->glyph_height,
      font->glyph_stride = header->bytes_per_glyph;
      font->glyphs = (root_uint8_t *) (header + 1),
      font->put_glyph = root_psf_put_glyph;
    }
  else
    return ROOT_EINVAL;
  return ROOT_SUCCESS;
}

void *
root_psf_get_glyph (const root_font_t *font, root_uint8_t glyph)
{
  if (font->uc == NULL)
    return font->glyphs + glyph * font->glyph_stride;
  return font->glyphs + font->uc[glyph] * font->glyph_stride;
}

void
root_psf_put_glyph (const root_font_t *font, root_uint8_t *dst,
                    root_uint8_t glyph, root_uint32_t fg, root_uint32_t bg,
                    const root_gfx_mode_t *gfx_mode)
{
  root_size_t bytes_per_row = (font->glyph_width + 7) >> 3;
  root_uint8_t *glyph_data = root_psf_get_glyph (font, glyph);
  for (root_uint16_t y = 0; y < font->glyph_height; y++)
    {
      for (root_uint16_t x = 0; x < font->glyph_width; x++)
        {
          root_size_t i = y * gfx_mode->stride + x * gfx_mode->bpp;
          root_uint32_t color = bg;
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
