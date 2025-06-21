#include "gfx.h"
#include "psf2.h"
#include "string.h"
#include "text.h"

static root_u16 gfxgetwidth (root_term_t *term);
static root_u16 gfxgetheight (root_term_t *term);
static void gfxputchar (root_term_t *term, char ch);
static void gfxputchar_unsynced (root_term_t *term, char ch);
static void gfxadvance (root_term_t *term, root_u16 x, root_u16 y);
static void gfxsetcursorpos (struct root_term_t *term, root_u16 x, root_u16 y);
static void gfxsetcursorvisible (struct root_term_t *term, root_u8 visible);
static void gfxsetcursorcolor (struct root_term_t *term, root_u32 fg,
                               root_u32 bg);
static void gfxclear (root_term_t *term, root_u32 color);

int
root_initgfx_term (root_gfx_term_t *term, root_video_mode_t *video_mode)
{
  if (term == NULL || video_mode == NULL)
    return -1;
  term->base.type = ROOT_TERM_TYPE_GFX;
  term->base.attribs
      = (root_term_attribs_t){ .scrolltype = ROOT_SCROLL_TYPE_NEWLINE,
                               .tabsize = 4 };
  term->base.pos.x = 0;
  term->base.pos.y = 0;
  term->base.cursor.x = 0;
  term->base.cursor.y = 0;
  term->base.cursor.visible = 0;
  term->base.getwidth = gfxgetwidth;
  term->base.getheight = gfxgetheight;
  term->base.putchar = gfxputchar;
  term->base.putchar_unsynced = gfxputchar_unsynced;
  term->base.advance = gfxadvance;
  term->base.setcursorpos = gfxsetcursorpos;
  term->base.setcursorvisible = gfxsetcursorvisible;
  term->base.setcursorcolor = gfxsetcursorcolor;
  term->base.clear = gfxclear;
  term->video_mode = *video_mode;
  term->cursor_fg = 0x0000CC00;
  term->cursor_bg = 0x00000000;
  return 0;
}

root_u16
gfxgetwidth (root_term_t *term)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  return gfxterm->video_mode.width / 10;
}

root_u16
gfxgetheight (root_term_t *term)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  return gfxterm->video_mode.height / 20;
}

void
gfxputchar (root_term_t *term, char ch)
{
  gfxputchar_unsynced (term, ch);
}

void
gfxputchar_unsynced (root_term_t *term, char ch)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  switch (ch)
    {
    case '\t':
      for (root_u32 i = 0; i < gfxterm->base.attribs.tabsize; i++)
        gfxputchar_unsynced (term, ' ');
      break;
    case '\r':
      gfxadvance (term, 0, 1);
      break;
    case '\n':
      gfxadvance (term, ROOT_TEXT_POS_RESET, 1);
      break;
    default:
      {
        root_u8 *fb = gfxterm->video_mode.framebuffer
                      + gfxterm->video_mode.pitch * term->pos.y * 20
                      + (gfxterm->video_mode.bpp >> 3) * term->pos.x * 10;
        root_psf2_putglyph (fb, ch, gfxterm->cursor_fg, gfxterm->cursor_bg,
                            gfxterm->video_mode.pitch);
        gfxadvance (term, 1, 0);
        break;
      }
    }
}

void
gfxadvance (root_term_t *term, root_u16 x, root_u16 y)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  root_u16 nx = gfxterm->base.pos.x;
  root_u16 ny = gfxterm->base.pos.y;

  if (x != ROOT_TEXT_POS_RESET)
    {
      nx += x;
      ny += nx / gfxterm->video_mode.width;
      nx %= gfxterm->video_mode.width;
    }
  else
    {
      nx = 0;
    }
  if (y != ROOT_TEXT_POS_RESET)
    {
      ny += y;
      switch (gfxterm->base.attribs.scrolltype)
        {
        case ROOT_SCROLL_TYPE_WRAP:
          ny %= gfxterm->video_mode.height;
          break;
        case ROOT_SCROLL_TYPE_NEWLINE:
          if (ny >= gfxterm->video_mode.height)
            {
              root_u16 rows = ny - gfxterm->video_mode.height + 1,
                       clearstart = gfxterm->video_mode.height - rows;
              root_u8 *src = gfxterm->video_mode.framebuffer
                             + rows * gfxterm->video_mode.pitch,
                      *dst = gfxterm->video_mode.framebuffer;
              (void) clearstart;
              (void) src;
              (void) dst;
            }
          break;
        }
    }
  else if (y == ROOT_TEXT_POS_RESET)
    ny = 0;

  gfxterm->base.pos.x = nx;
  gfxterm->base.pos.y = ny;
}

void
gfxsetcursorpos (struct root_term_t *term, root_u16 x, root_u16 y)
{
  (void) term;
  (void) x;
  (void) y;
}

void
gfxsetcursorvisible (struct root_term_t *term, root_u8 visible)
{
  (void) term;
  (void) visible;
}

void
gfxsetcursorcolor (struct root_term_t *term, root_u32 fg, root_u32 bg)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  gfxterm->cursor_fg = fg;
  gfxterm->cursor_bg = bg;
}

void
gfxclear (root_term_t *term, root_u32 color)
{
  root_gfx_term_t *gfxterm = (root_gfx_term_t *) term;
  root_memset (gfxterm->video_mode.framebuffer, 0,
               gfxterm->video_mode.height * gfxterm->video_mode.pitch);
  (void) color;
}
