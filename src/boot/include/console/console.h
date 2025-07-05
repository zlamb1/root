#include "io/fd.h"
#ifndef ROOT_CONSOLE_H
#define ROOT_CONSOLE_H 1

#include "types.h"

#define ROOT_CONSOLE_POS_RESET (-0x8000)

typedef struct root_cursor_t
{
  root_u16 x, y;
  root_u8 enabled;
} root_cursor_t;

typedef struct root_console_buf_t
{
  root_size_t sz, cap, cursor;
  char *buf;
} root_console_buf_t;

typedef struct root_console_fd_t
{
  root_fd_t fd;
  struct root_console_t *con;
} root_console_fd_t;

typedef struct root_console_t
{
  root_u16 width, height, scroll_height;
  root_u16 head, offset;
  root_u16 x, y, ocx, ocy;
  root_u8 cursor_enabled;
  root_u8 tab_size;
  root_u32 fg, bg;
  root_console_buf_t in;
  root_console_fd_t fd;
  /* impl interface */
  void (*putglyph) (struct root_console_t *con, char ch, root_u16 x,
                    root_u16 y);
  void (*putvrow) (struct root_console_t *con, root_u16 vrow, root_u16 row);
  void (*fillvrow) (struct root_console_t *con, root_u16 vrow, root_u32 bg);
  void (*putcursor) (struct root_console_t *con, root_u16 x, root_u16 y);
  void (*setcursor) (struct root_console_t *con, root_u8 enabled);
  void (*blinkcursor) (struct root_console_t *con);
  /* generic interface */
  void (*putchar) (struct root_console_t *con, char ch);
  void (*moveto) (struct root_console_t *con, root_u16 x, root_u16 y);
  void (*advance) (struct root_console_t *con);
  void (*newline) (struct root_console_t *con);
  void (*clear) (struct root_console_t *con, root_u32 bg);
} root_console_t;

root_err_t console_init (struct root_console_t *con);

static inline void
console_sync_cursor (root_console_t *con)
{
  con->putcursor (con, con->x, con->y);
  con->blinkcursor (con);
}

static inline void
console_putchar_sync (root_console_t *con, char ch)
{
  con->putchar (con, ch);
  console_sync_cursor (con);
}

void console_putchar (struct root_console_t *con, char ch);
void console_moveto (root_console_t *con, root_u16 x, root_u16 y);
void console_advance (root_console_t *con);
void console_newline (root_console_t *con);
void console_clear (root_console_t *con, root_u32 bg);

void console_offsetup (root_console_t *con);
void console_offsetdown (root_console_t *con);

static inline root_fd_t *
console_fd (root_console_t *con)
{
  return &con->fd.fd;
}

static inline root_ssize_t
console_read (root_console_t *con, char *buf, root_size_t sz)
{
  return fread (&con->fd.fd, buf, sz);
}

static inline root_ssize_t
console_write (root_console_t *con, const char *buf, root_size_t sz)
{
  return fwrite (&con->fd.fd, buf, sz);
}

#endif
