#ifndef ROOT_GFX_H
#define ROOT_GFX_H

#include "console.h"
#include "font.h"
#include "gfx_mode.h"
#include "io/fd.h"
#include "task.h"
#include "types.h"

typedef struct root_gfx_fd_t
{
  root_fd_t fd;
  struct root_gfx_console_t *con;
} root_gfx_fd_t;

typedef struct root_gfx_console_t
{
  root_console_t base;
  root_gfx_mode_t gfx_mode;
  root_font_t font;
  root_gfx_fd_t fd;
  root_u8 *fb;
  root_u8 *bb;
  root_u8 cursor_on;
  root_task_t *cursor_task;
} root_gfx_console_t;

root_err_t gfx_console_init (root_gfx_console_t *con,
                             root_gfx_mode_t *gfx_mode, root_font_t *font);

void gfx_putglyph (struct root_console_t *con, char ch, root_u16 x,
                   root_u16 y);
void gfx_putvrow (struct root_console_t *con, root_u16 vrow, root_u16 row);
void gfx_fillvrow (struct root_console_t *con, root_u16 vrow, root_u32 bg);
void gfx_putcursor (struct root_console_t *con, root_u16 x, root_u16 y);
void gfx_setcursor (struct root_console_t *con, root_u8 enabled);
void gfx_blinkcursor (struct root_console_t *con);
root_fd_t *gfx_open (struct root_console_t *con);

#endif
