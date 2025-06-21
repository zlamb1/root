#include "vga.h"
#include "machine_io.h"
#include "text.h"

#define MISC_OUTPUT_READ_REG     0x3CC
#define MISC_OUTPUT_WRITE_REG    0x3C2
#define CRTC_ADDRESS_REG         0x3D4
#define CRTC_DATA_REG            0x3D5
#define CRTC_MAX_SCANLINE_REG    0x009
#define CRTC_CURSOR_START_REG    0x00A
#define CRTC_CURSOR_END_REG      0x00B
#define CRTC_CURSOR_HIGH_LOC_REG 0x00E
#define CRTC_CURSOR_LOW_LOC_REG  0x00F

static root_u16 vgagetwidth (root_term_t *term);
static root_u16 vgagetheight (root_term_t *term);
static void vgaputchar (root_term_t *term, char c);
static void vgaputchar_unsynced (root_term_t *term, char c);
static void vgaadvance (root_term_t *term, root_u16 x, root_u16 y);
static void vgasetcursorpos (root_term_t *term, root_u16 x, root_u16 y);
static void vgasetcursorvisible (root_term_t *term, root_u8 visible);
static void vgasetcursorcolor (root_term_t *term, root_u32 fg, root_u32 bg);
static void vgaclear (root_term_t *term, root_u32 color);

int
root_initvga (root_vga_term_t *out)
{
  root_u8 fg = ROOT_VGA_COLOR_GREEN;
  root_u8 bg = ROOT_VGA_COLOR_BLACK;

  out->base.pos.x = 0;
  out->base.pos.y = 0;
  out->base.cursor.x = 0;
  out->base.cursor.y = 0;
  out->base.cursor.visible = 0;

  out->base.type = ROOT_TERM_TYPE_VGA;
  out->base.attribs.tabsize = 4;
  out->base.attribs.scrolltype = ROOT_SCROLL_TYPE_NEWLINE;

  out->base.getwidth = vgagetwidth;
  out->base.getheight = vgagetheight;

  out->base.putchar = vgaputchar;
  out->base.putchar_unsynced = vgaputchar_unsynced;
  out->base.advance = vgaadvance;
  out->base.setcursorpos = vgasetcursorpos;
  out->base.setcursorvisible = vgasetcursorvisible;
  out->base.setcursorcolor = vgasetcursorcolor;
  out->base.clear = vgaclear;

  out->state.width = 80;
  out->state.height = 25;
  out->state.stride = out->state.width << 1;
  out->state.data = (root_u8 *) 0xB8000;

  out->cursor_color = bg << 4 | fg;

  root_outb (CRTC_ADDRESS_REG, CRTC_MAX_SCANLINE_REG);
  out->state.max_scanline = root_inb (CRTC_DATA_REG) & 0x1F;

  vgaclear (&out->base, bg << 4 | fg);
  vgasetcursorpos (&out->base, out->base.cursor.x, out->base.cursor.y);
  vgasetcursorvisible (&out->base, 1);

  return 0;
}

static root_u16
vgagetwidth (root_term_t *term)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  return vga_term->state.width;
}

static root_u16
vgagetheight (root_term_t *term)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  return vga_term->state.height;
}

void
vgaputchar (root_term_t *term, char c)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  vgaputchar_unsynced (term, c);
  vgasetcursorpos (term, vga_term->base.pos.x, vga_term->base.pos.y);
}

static void
vgaputchar_unsynced (root_term_t *term, char c)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  root_u8 *data = vga_term->state.data;
  data += vga_term->base.pos.y * vga_term->state.stride
          + (vga_term->base.pos.x << 1);
  switch (c)
    {
    case '\t':
      for (root_u32 i = 0; i < vga_term->base.attribs.tabsize; i++)
        vgaputchar_unsynced (term, ' ');
      break;
    case '\r':
      vgaadvance (term, 0, 1);
      break;
    case '\n':
      vgaadvance (term, ROOT_TEXT_POS_RESET, 1);
      break;
    default:
      data[0] = (root_u8) c;
      data[1] = vga_term->cursor_color;
      vgaadvance (term, 1, 0);
      break;
    }
}

void
vgaadvance (root_term_t *term, root_u16 x, root_u16 y)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  root_u16 nx = vga_term->base.pos.x;
  root_u16 ny = vga_term->base.pos.y;

  if (x != ROOT_TEXT_POS_RESET)
    {
      nx += x;
      ny += nx / vga_term->state.width;
      nx %= vga_term->state.width;
    }
  else
    {
      nx = 0;
    }
  if (y != ROOT_TEXT_POS_RESET)
    {
      ny += y;
      switch (vga_term->base.attribs.scrolltype)
        {
        case ROOT_SCROLL_TYPE_WRAP:
          ny %= vga_term->state.height;
          break;
        case ROOT_SCROLL_TYPE_NEWLINE:
          if (ny >= vga_term->state.height)
            {
              root_u16 rows = ny - vga_term->state.height + 1,
                       clearstart = vga_term->state.height - rows;
              root_u8 *src
                  = vga_term->state.data + rows * vga_term->state.stride,
                  *dst = vga_term->state.data;
              for (root_u16 ry = 0; ry < clearstart; ry++)
                {
                  for (root_u16 rx = 0; rx < vga_term->state.width; rx++)
                    {
                      root_u16 idx = rx << 1;
                      dst[idx] = src[idx];
                      dst[idx + 1] = src[idx + 1];
                    }
                  src += vga_term->state.stride;
                  dst += vga_term->state.stride;
                }
              dst = vga_term->state.data + clearstart * vga_term->state.stride;
              for (root_u16 ry = clearstart; ry < vga_term->state.height; ry++)
                {
                  for (root_u16 rx = 0; rx < vga_term->state.width;
                       rx++, dst += 2)
                    {
                      dst[0] = 0;
                      dst[1] = vga_term->cursor_color;
                    }
                }
              ny = vga_term->state.height - 1;
            }
          break;
        }
    }
  else if (y == ROOT_TEXT_POS_RESET)
    ny = 0;

  vga_term->base.pos.x = nx;
  vga_term->base.pos.y = ny;
}

void
vgasetcursorpos (root_term_t *term, root_u16 x, root_u16 y)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  root_u16 pos = y * vga_term->state.width + x;
  vga_term->base.cursor.x = x;
  vga_term->base.cursor.y = y;
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG);
  root_outb (CRTC_DATA_REG, pos & 0xFF);
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG);
  root_outb (CRTC_DATA_REG, (pos >> 8) & 0xFF);
}

void
vgasetcursorvisible (root_term_t *term, root_u8 visible)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  if (vga_term->base.cursor.visible != visible)
    {
      if (visible == 1)
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, (root_inb (CRTC_DATA_REG) & 0xC0));
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_END_REG);
          root_outb (CRTC_DATA_REG, (root_inb (CRTC_DATA_REG) & 0xE0)
                                        | vga_term->state.max_scanline);
        }
      else
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, 0x20);
        }
      vga_term->base.cursor.visible = visible;
    }
}

void
vgasetcursorcolor (root_term_t *term, root_u32 fg, root_u32 bg)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  vga_term->cursor_color = (fg & 0xF) | (bg & 0xF) << 4;
}

void
vgaclear (root_term_t *term, root_u32 color)
{
  root_vga_term_t *vga_term = (root_vga_term_t *) term;
  root_u8 *data = vga_term->state.data;
  for (root_u16 y = 0; y < vga_term->state.height; y++)
    {
      for (root_u16 x = 0; x < vga_term->state.width; x++, data += 2)
        {
          data[0] = 0;
          data[1] = color;
        }
    }
}
