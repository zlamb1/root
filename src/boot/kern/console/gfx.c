#include "console/gfx.h"
#include "console/console.h"
#include "console/font.h"
#include "console/gfx_mode.h"
#include "memory/alloc.h"
#include "memory/page.h"
#include "panic.h"
#include "string.h"
#include "task.h"
#include "types.h"

#define GFX_SCROLL_ROWS            300
#define GFX_CURSOR_BLINK_PERIOD_MS 500

static void gfx_cursor_task (void *con);

static void
match_font_and_gfx_size (root_gfx_console_t *con)
{
  con->base.width = con->gfx_mode.width / con->font.glyph_width;
  con->base.height = con->gfx_mode.height / con->font.glyph_height;
}

static void
gfx_drawcursor (root_gfx_console_t *con, int cursor_on, root_u16 x, root_u16 y)
{
  if (y >= con->base.offset && y - con->base.offset < con->base.height)
    {
      root_gfx_mode_t *gfx_mode = &con->gfx_mode;
      root_font_t *font = &con->font;
      root_u8 *fb
          = gfx_mode->fb
            + gfx_mode->stride * font->glyph_height * (y - con->base.offset)
            + gfx_mode->bpp * font->glyph_width * x;
      // TODO: SAVE FB CONTENTS!
      root_u32 color = cursor_on ? con->base.fg : con->base.bg;
      for (root_u32 cy = 0; cy < font->glyph_height; cy++)
        {
          fb[0] = color;
          fb[1] = color >> 8;
          fb[2] = color >> 16;
          fb += gfx_mode->stride;
        }
    }
}

static void
gfx_resetcursor (root_gfx_console_t *con)
{
  con->cursor_task->ticker = 0;
  if (con->cursor_on)
    {
      gfx_drawcursor (con, 0, con->base.ocx, con->base.ocy);
      con->cursor_on = 0;
    }
}

root_err_t
gfx_console_init (root_gfx_console_t *con, root_gfx_mode_t *gfx_mode,
                  root_font_t *font)
{
  root_err_t error = ROOT_ERR_ARG;
  root_size_t pages;
  root_u8 *backbuffer;
  if (con == NULL || gfx_mode == NULL || font == NULL
      || (error = console_init (&con->base)) != ROOT_SUCCESS)
    return error;
  pages = ROOT_PAGE_ALIGN_UP (gfx_mode->width * gfx_mode->bpp
                              * font->glyph_height * GFX_SCROLL_ROWS)
          >> ROOT_PAGE_POW;
  backbuffer = root_alloc_pages (pages);
  // TODO: create non-backbuffer functions
  if (backbuffer == NULL)
    return ROOT_ERR_ALLOC;
  root_memset (backbuffer, 0, pages << ROOT_PAGE_POW);
  con->fb = gfx_mode->fb;
  con->bb = backbuffer;
  con->gfx_mode = *gfx_mode;
  con->font = *font;
  match_font_and_gfx_size (con);
  con->base.scroll_height = GFX_SCROLL_ROWS;
  con->base.fg = 0x00AA00;
  con->base.bg = 0;
  con->base.putglyph = gfx_putglyph;
  con->base.putvrow = gfx_putvrow;
  con->base.fillvrow = gfx_fillvrow;
  con->base.putcursor = gfx_putcursor;
  con->base.setcursor = gfx_setcursor;
  con->base.blinkcursor = gfx_blinkcursor;
  con->base.setcursor (&con->base, 1);
  con->cursor_task = root_register_task (GFX_CURSOR_BLINK_PERIOD_MS,
                                         gfx_cursor_task, &con->base);
  return ROOT_SUCCESS;
}

void
gfx_putglyph (root_console_t *con, char ch, root_u16 x, root_u16 y)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  root_font_t *font = &gfx_con->font;
  root_size_t rowsize = gfx_con->gfx_mode.width * gfx_con->gfx_mode.bpp;
  root_u8 *bb
      = gfx_con->bb
        + ((y + con->head) % con->scroll_height) * rowsize * font->glyph_height
        + x * gfx_con->gfx_mode.bpp * font->glyph_width;
  gfx_con->font.put_glyph (font, bb, ch, con->fg, con->bg, &gfx_con->gfx_mode);
  if (y >= con->offset && y - con->offset < con->height)
    {
      root_u8 *fb = gfx_con->gfx_mode.fb
                    + ((y - con->offset) * gfx_con->gfx_mode.stride
                       * font->glyph_height)
                    + (x * gfx_con->gfx_mode.bpp * font->glyph_width);
      for (root_u16 row = 0; row < font->glyph_height; row++)
        {
          root_memcpy (fb, bb, gfx_con->gfx_mode.bpp * font->glyph_width);
          bb += rowsize;
          fb += gfx_con->gfx_mode.stride;
        }
    }
}

void
gfx_putvrow (struct root_console_t *con, root_u16 vrow, root_u16 row)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  root_u8 *src, *dst;
  root_size_t rowsize, fh;
  if (vrow >= con->scroll_height)
    root_panic ("gfx: invalid vrow");
  if (row >= con->height)
    root_panic ("gfx: invalid row");
  if (gfx_con->bb == NULL)
    root_panic ("gfx: invalid backbuffer");
  rowsize = gfx_con->gfx_mode.width * gfx_con->gfx_mode.bpp;
  fh = gfx_con->font.glyph_height;
  src = gfx_con->bb + rowsize * vrow * fh;
  dst = gfx_con->fb + gfx_con->gfx_mode.stride * row * fh;
  for (root_size_t i = 0; i < fh; i++)
    {
      root_memcpy (dst, src, rowsize);
      src += rowsize;
      dst += gfx_con->gfx_mode.stride;
    }
}

void
gfx_fillvrow (struct root_console_t *con, root_u16 vrow, root_u32 bg)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  root_u8 *dst;
  root_size_t rowsize, fh;
  if (vrow >= con->scroll_height)
    root_panic ("gfx: invalid vrow");
  if (gfx_con->bb == NULL)
    root_panic ("gfx: invalid backbuffer");
  rowsize = gfx_con->gfx_mode.width * gfx_con->gfx_mode.bpp;
  fh = gfx_con->font.glyph_height;
  dst = gfx_con->bb + rowsize * vrow * fh;
  root_memsetdw (dst, bg, fh * gfx_con->gfx_mode.width);
}

void
gfx_putcursor (root_console_t *con, root_u16 x, root_u16 y)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  gfx_resetcursor (gfx_con);
  con->ocx = x;
  con->ocy = y;
  // reset animation
  if (con->cursor_enabled)
    {
      gfx_con->cursor_task->ticker = 0;
      gfx_drawcursor (gfx_con, 1, x, y);
      gfx_con->cursor_on = 1;
    }
}

void
gfx_setcursor (root_console_t *con, root_u8 enabled)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  gfx_con->cursor_task->ticker = 0;
  if (con->cursor_enabled != enabled)
    {
      gfx_resetcursor (gfx_con);
      con->cursor_enabled = enabled;
    }
}

void
gfx_blinkcursor (struct root_console_t *con)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  if (gfx_con->cursor_on)
    gfx_con->cursor_task->ticker = 0;
  else
    {
      gfx_con->cursor_on = 1;
      gfx_con->cursor_task->ticker = GFX_CURSOR_BLINK_PERIOD_MS;
    }
}

void
gfx_cursor_task (void *con)
{
  root_gfx_console_t *gfx_con = (root_gfx_console_t *) con;
  if (gfx_con->base.cursor_enabled)
    {
      root_u8 cursor_on = !gfx_con->cursor_on;
      gfx_drawcursor (gfx_con, cursor_on, gfx_con->base.ocx,
                      gfx_con->base.ocy);
      gfx_con->cursor_on = cursor_on;
    }
}
