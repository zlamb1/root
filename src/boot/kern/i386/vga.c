#include "i386/vga.h"
#include "console/console.h"
#include "i386/machine_io.h"
#include "string.h"

#define MISC_OUTPUT_READ_REG     0x3CC
#define MISC_OUTPUT_WRITE_REG    0x3C2
#define CRTC_ADDRESS_REG         0x3D4
#define CRTC_DATA_REG            0x3D5
#define CRTC_MAX_SCANLINE_REG    0x009
#define CRTC_CURSOR_START_REG    0x00A
#define CRTC_CURSOR_END_REG      0x00B
#define CRTC_CURSOR_HIGH_LOC_REG 0x00E
#define CRTC_CURSOR_LOW_LOC_REG  0x00F

static root_u8 *
root_get_fb (root_u8 *fb, root_u16 x, root_u16 y, root_u16 pitch)
{
  return fb + (y * pitch) + (x << 1);
}

root_err_t
vga_console_init (root_vga_console_t *con)
{
  root_err_t err = ROOT_ERR_ARG;
  if (con == NULL || (err = console_init (&con->base)) != ROOT_SUCCESS)
    return err;
  root_outb (CRTC_ADDRESS_REG, CRTC_MAX_SCANLINE_REG);
  con->max_scanline = root_inb (CRTC_DATA_REG) & 0x1F;
  con->fb = (root_u8 *) 0xB8000;
  con->base.width = 80;
  con->base.height = 25;
  con->base.scroll_height = 25;
  con->base.fg = ROOT_VGA_COLOR_GREEN;
  con->base.bg = ROOT_VGA_COLOR_BLACK;
  con->base.putglyph = vga_putglyph;
  con->base.putvrow = vga_putvrow;
  con->base.fillvrow = vga_fillvrow;
  con->base.putcursor = vga_putcursor;
  con->base.setcursor = vga_setcursor;
  con->base.blinkcursor = vga_blinkcursor;
  con->base.scroll = vga_scroll;
  return ROOT_SUCCESS;
}

void
vga_putglyph (struct root_console_t *con, char ch, root_u16 x, root_u16 y)
{
  root_vga_console_t *vga = (root_vga_console_t *) con;
  root_u8 *fb = root_get_fb (vga->fb, x, y, con->width << 1);
  fb[0] = ch;
  fb[1] = (con->fg & 0xF) | ((con->bg & 0xF) << 4);
}

void
vga_putvrow (struct root_console_t *con, root_u16 vrow, root_u16 row)
{
  (void) con;
  (void) vrow;
  (void) row;
}

void
vga_fillvrow (struct root_console_t *con, root_u16 vrow, root_u32 bg)
{
  root_vga_console_t *vga = (root_vga_console_t *) con;
  root_u8 *fb = root_get_fb (vga->fb, 0, vrow, con->width << 1);
  root_u16 val = ((con->fg & 0xF) | ((bg & 0xF) << 4)) << 8;
  root_memsetw (fb, val, con->width);
}

void
vga_putcursor (struct root_console_t *con, root_u16 x, root_u16 y)
{
  root_u16 pos = y * con->width + x;
  con->ocx = x;
  con->ocy = y;
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG);
  root_outb (CRTC_DATA_REG, pos & 0xFF);
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG);
  root_outb (CRTC_DATA_REG, (pos >> 8) & 0xFF);
}

void
vga_setcursor (struct root_console_t *con, root_u8 enabled)
{
  root_vga_console_t *vga = (root_vga_console_t *) con;
  if (con->cursor_enabled != enabled)
    {
      if (enabled)
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, (root_inb (CRTC_DATA_REG) & 0xC0));
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_END_REG);
          root_outb (CRTC_DATA_REG,
                     (root_inb (CRTC_DATA_REG) & 0xE0) | vga->max_scanline);
        }
      else
        {
          root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_START_REG);
          root_outb (CRTC_DATA_REG, 0x20);
        }
      con->cursor_enabled = enabled;
    }
}

void
vga_blinkcursor (struct root_console_t *con)
{
  (void) con;
}

void
vga_scroll (struct root_console_t *con)
{
  root_vga_console_t *vga = (root_vga_console_t *) con;
  root_size_t rowsize = con->width << 1;
  root_memmove (vga->fb, vga->fb + rowsize, rowsize * con->scroll_height - 1);
  con->y = con->scroll_height - 1;
}
