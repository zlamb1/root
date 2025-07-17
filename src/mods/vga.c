#include "vga.h"
#include "errno.h"
#include "machine.h"
#include "mod.h"
#include "string.h"
#include "term.h"
#include "types.h"

#define VMEM 0xB8000
#define COLS 80
#define ROWS 25

#define MISC_OUTPUT_READ_REG     0x3CC
#define MISC_OUTPUT_WRITE_REG    0x3C2
#define CRTC_ADDRESS_REG         0x3D4
#define CRTC_DATA_REG            0x3D5
#define CRTC_MAX_SCANLINE_REG    0x009
#define CRTC_CURSOR_START_REG    0x00A
#define CRTC_CURSOR_END_REG      0x00B
#define CRTC_CURSOR_HIGH_LOC_REG 0x00E
#define CRTC_CURSOR_LOW_LOC_REG  0x00F

#define VGA_TERM(T)                                                           \
  vga_term_t *vga_term;                                                       \
  if ((T) == NULL)                                                            \
    {                                                                         \
      root_seterrno (ROOT_EARG);                                              \
      return;                                                                 \
    }                                                                         \
  vga_term = (vga_term_t *) (T);

#define VGA_TERM_RET(T, R)                                                    \
  vga_term_t *vga_term;                                                       \
  if ((T) == NULL)                                                            \
    {                                                                         \
      root_seterrno (ROOT_EARG);                                              \
      return (R);                                                             \
    }                                                                         \
  vga_term = (vga_term_t *) (T);

typedef struct
{
  root_term_t base;
  root_uint16_t x, y, cx, cy;
  root_uint8_t fg, bg;
  root_file_t stdout;
} vga_term_t;

static vga_term_t term = { .base = { .init = vga_init,
                                     .putchar = vga_putchar,
                                     .putcursor = vga_putcursor,
                                     .getfg = vga_getfg,
                                     .setfg = vga_setfg,
                                     .getbg = vga_getbg,
                                     .setbg = vga_setbg,
                                     .getwh = vga_getwh,
                                     .getxy = vga_getxy,
                                     .setxy = vga_setxy,
                                     .advance = vga_advance,
                                     .newline = vga_newline,
                                     .clr = vga_clr } };

void
vga_init (root_term_t *t)
{
  VGA_TERM (t);
  t->putcursor (t, vga_term->cx, vga_term->cy);
  t->clr (t);
}

void
vga_putchar (root_term_t *t, char ch)
{
  root_uint8_t *vmem;
  root_uint8_t clr;
  VGA_TERM (t);
  clr = (vga_term->fg & 0xF) | (vga_term->bg & 0xF) << 4;
  vmem = (root_uint8_t *) (VMEM + vga_term->y * (COLS << 1)
                           + (vga_term->x << 1));
  vmem[0] = ch;
  vmem[1] = clr;
}

void
vga_putcursor (root_term_t *t, root_uint16_t x, root_uint16_t y)
{
  root_uint16_t pos;
  VGA_TERM (t);
  pos = y * COLS + x;
  vga_term->cx = x;
  vga_term->cy = y;
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG);
  root_outb (CRTC_DATA_REG, pos & 0xFF);
  root_outb (CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG);
  root_outb (CRTC_DATA_REG, (pos >> 8) & 0xFF);
}

root_uint32_t
vga_getfg (root_term_t *t)
{
  VGA_TERM_RET (t, 0);
  return vga_term->fg;
}

void
vga_setfg (root_term_t *t, root_uint32_t fg)
{
  VGA_TERM (t);
  vga_term->fg = fg;
}

root_uint32_t
vga_getbg (root_term_t *t)
{
  VGA_TERM_RET (t, 0);
  return vga_term->bg;
}

void
vga_setbg (root_term_t *t, root_uint32_t bg)
{
  VGA_TERM (t);
  vga_term->bg = bg;
}

root_term_size_t
vga_getwh (root_term_t *t)
{
  VGA_TERM_RET (t, (root_term_size_t){ 0 });
  return (root_term_size_t){ .width = COLS, .height = ROWS };
}

root_term_pos_t
vga_getxy (root_term_t *t)
{
  VGA_TERM_RET (t, (root_term_pos_t){ 0 });
  return (root_term_pos_t){ .x = vga_term->x, .y = vga_term->y };
}

void
vga_setxy (root_term_t *t, root_uint16_t x, root_uint16_t y)
{
  VGA_TERM (t);
  vga_term->x = x;
  vga_term->y = y;
}

void
vga_advance (root_term_t *t)
{
  VGA_TERM (t);
  vga_term->x++;
  if (vga_term->x >= COLS)
    {
      vga_term->x = 0;
      t->newline (t);
    }
}

void
vga_newline (struct root_term_t *t)
{
  VGA_TERM (t);
  vga_term->y++;
  if (vga_term->y >= ROWS)
    vga_term->y = 0;
}

void
vga_clr (root_term_t *t)
{
  root_uint16_t clr;
  VGA_TERM (t);
  clr = (vga_term->fg & 0xF) | (vga_term->bg & 0xF) << 4;
  root_memsetw ((void *) VMEM, clr << 8, COLS * ROWS);
}

void
root_vga_term_mod_init (void)
{
  term.x = 0;
  term.y = 0;
  term.cx = 0;
  term.cy = 0;
  term.fg = VGA_COLOR_WHITE;
  term.bg = VGA_COLOR_BLACK;
  root_seterrno (ROOT_SUCCESS);
  root_term_init (&term.base);
  if (root_errno != ROOT_SUCCESS)
    // TODO: error
    return;
  root_register_term (&term.base);
}

void
root_vga_term_mod_fini (void)
{
  root_unregister_term (&term.base);
}

ROOT_MOD_INIT (vga_term) { root_vga_term_mod_init (); }

ROOT_MOD_FINI () { root_vga_term_mod_fini (); }
