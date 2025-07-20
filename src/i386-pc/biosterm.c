#include "i386-pc/biosterm.h"
#include "i386-pc/bios.h"
#include "kern/errno.h"
#include "kern/term.h"

#define COLS 80
#define ROWS 25

#define BIOS_TERM(T)                                                          \
  bios_term_t *bios_term;                                                     \
  if ((T) == NULL)                                                            \
    {                                                                         \
      root_seterrno (ROOT_EARG);                                              \
      return;                                                                 \
    }                                                                         \
  bios_term = (bios_term_t *) (T);

#define BIOS_TERM_RET(T, R)                                                   \
  bios_term_t *bios_term;                                                     \
  if ((T) == NULL)                                                            \
    {                                                                         \
      root_seterrno (ROOT_EARG);                                              \
      return (R);                                                             \
    }                                                                         \
  bios_term = (bios_term_t *) (T);

typedef struct
{
  root_term_t base;
  root_uint16_t x, y, cx, cy;
  root_uint8_t fg, bg;
  root_file_t stdout;
} bios_term_t;

static bios_term_t term = { .base = { .putchar = bios_term_putchar,
                                      .putcursor = bios_term_putcursor,
                                      .getfg = bios_term_getfg,
                                      .setfg = bios_term_setfg,
                                      .getbg = bios_term_getbg,
                                      .setbg = bios_term_setbg,
                                      .getwh = bios_term_getwh,
                                      .getxy = bios_term_getxy,
                                      .setxy = bios_term_setxy,
                                      .advance = bios_term_advance,
                                      .newline = bios_term_newline,
                                      .clr = bios_term_clr } };

void
root_init_bios_term (void)
{
  term.x = 0;
  term.y = 0;
  term.cx = 0;
  term.cy = 0;
  term.fg = 0xF;
  term.bg = 0x0;
  root_seterrno (ROOT_SUCCESS);
  root_term_init (&term.base);
  if (root_errno != ROOT_SUCCESS)
    return;
  root_register_term (&term.base);
}

void
bios_term_putchar (root_term_t *t, char ch)
{
  root_bios_args_t args = { 0 };
  BIOS_TERM (t);
  args.eax = 0xE << 8 | ch;
  args.ebx = bios_term->fg & 0xFF;
  root_bios_interrupt (0x10, &args);
}

void
bios_term_putcursor (root_term_t *t, root_uint16_t x, root_uint16_t y)
{
  root_uint16_t pos;
  root_bios_args_t args = { 0 };
  (void) t;
  args.eax = 0x2 << 8;
  args.edx = (y & 0xFF) << 8 | (x & 0xFF);
  root_bios_interrupt (0x10, &args);
}

root_uint32_t
bios_term_getfg (root_term_t *t)
{
  BIOS_TERM_RET (t, 0);
  return bios_term->fg;
}

void
bios_term_setfg (root_term_t *t, root_uint32_t fg)
{
  BIOS_TERM (t);
  bios_term->fg = fg;
}

root_uint32_t
bios_term_getbg (root_term_t *t)
{
  BIOS_TERM_RET (t, 0);
  return bios_term->bg;
}

void
bios_term_setbg (root_term_t *t, root_uint32_t bg)
{
  BIOS_TERM (t);
  bios_term->bg = bg;
}

root_term_size_t
bios_term_getwh (root_term_t *t)
{
  BIOS_TERM_RET (t, (root_term_size_t){ 0 });
  return (root_term_size_t){ .width = COLS, .height = ROWS };
}

root_term_pos_t
bios_term_getxy (root_term_t *t)
{
  BIOS_TERM_RET (t, (root_term_pos_t){ 0 });
  return (root_term_pos_t){ .x = bios_term->x, .y = bios_term->y };
}

void
bios_term_setxy (root_term_t *t, root_uint16_t x, root_uint16_t y)
{
  BIOS_TERM (t);
  bios_term->x = x;
  bios_term->y = y;
}

void
bios_term_advance (root_term_t *t)
{
  BIOS_TERM (t);
  bios_term->x++;
  if (bios_term->x >= COLS)
    {
      bios_term->x = 0;
      t->newline (t);
    }
}

void
bios_term_newline (root_term_t *t)
{
  BIOS_TERM (t);
  bios_term->y++;
  if (ROWS > 0 && bios_term->y >= ROWS)
    {
      root_uint16_t clr = (bios_term->fg & 0xF) | (bios_term->bg & 0xF) << 4;
      bios_term->y = ROWS - 1;
    }
}

void
bios_term_clr (root_term_t *t)
{
  root_uint16_t clr;
  BIOS_TERM (t);
  t->putcursor (t, 0, 0);
  for (int i = 0; i < ROWS * COLS; i++)
    t->putchar (t, ' ');
  t->putcursor (t, 0, 0);
}
