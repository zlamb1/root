#include "console/console.h"
#include "input.h"
#include "input/keycode.h"
#include "memory/alloc.h"
#include "memory/page.h"
#include "panic.h"
#include "string.h"
#include "task.h"
#include "types.h"

static root_ssize_t _console_read (root_fd_t *fd, char *buf, root_size_t sz);
static root_ssize_t _console_write (root_fd_t *fd, const char *buf,
                                    root_size_t sz);

root_err_t
console_init (root_console_t *con)
{
  con->head = 0;
  con->offset = 0;
  con->x = 0;
  con->y = 0;
  con->ocx = 0;
  con->ocy = 0;
  con->fg = 0xFFFFFFFF;
  con->bg = 0;
  con->cursor_enabled = 0;
  con->tab_size = 4;
  con->in.sz = 0;
  con->in.cap = 0;
  con->in.cursor = 0;
  con->in.buf = NULL;
  con->fd.con = con;
  con->fd.fd.read = _console_read;
  con->fd.fd.write = _console_write;
  con->putchar = console_putchar;
  con->moveto = console_moveto;
  con->advance = console_advance;
  con->scroll = console_scroll;
  con->newline = console_newline;
  con->clear = console_clear;
  return ROOT_SUCCESS;
}

static inline void
console_draw (root_console_t *con)
{
  root_uint16_t vrow = (con->offset + con->head) % con->scroll_height;
  if (con->offset > con->scroll_height - con->height)
    root_panic ("console_draw: invalid offset");
  for (root_uint16_t row = 0; row < con->height; row++)
    {
      con->putvrow (con, vrow, row);
      vrow = (vrow + 1) % con->scroll_height;
    }
}

static inline int
console_setoffset (root_console_t *con)
{
  root_uint16_t offset = con->offset;
  con->offset = con->y >= con->height ? con->y - con->height + 1 : 0;
  if (con->offset + con->height > con->scroll_height)
    con->offset = con->scroll_height > con->height
                      ? con->scroll_height - con->height - 1
                      : 0;
  if (offset != con->offset)
    {
      console_draw (con);
      return 1;
    }
  return 0;
}

void
console_putchar (root_console_t *con, char ch)
{
  switch (ch)
    {
    case '\b':
      if (con->x)
        con->x--;
      else if (con->y)
        {
          con->x = con->width - 1;
          con->y--;
        }
      console_sync_cursor (con);
      break;
    case '\t':
      for (int i = 0; i < con->tab_size; i++)
        con->advance (con);
      break;
    case '\r':
      // TODO: don't reset x
      // fallthrough
    case '\n':
      con->newline (con);
      break;
    default:
      con->putglyph (con, ch, con->x, con->y);
      con->advance (con);
      break;
    }
}

void
console_moveto (root_console_t *con, root_uint16_t x, root_uint16_t y)
{
  if (x >= con->width || y >= con->scroll_height)
    root_panic ("console_moveto: invalid x/y");
  con->x = x;
  con->y = y;
  console_sync_cursor (con);
  console_setoffset (con);
}

void
console_advance (root_console_t *con)
{
  if (++con->x >= con->width)
    con->newline (con);
  else
    {
      console_sync_cursor (con);
      console_setoffset (con);
    }
}

void
console_scroll (root_console_t *con)
{
  con->y = con->scroll_height - 1;
  con->fillvrow (con, con->head, con->bg);
  con->head = (con->head + 1) % con->scroll_height;
  if (!console_setoffset (con))
    console_draw (con);
}

void
console_newline (root_console_t *con)
{
  con->x = 0;
  con->y++;
  if (con->y >= con->scroll_height)
    con->scroll (con);
  else
    console_setoffset (con);
  console_sync_cursor (con);
}

void
console_clear (root_console_t *con, root_uint32_t bg)
{
  for (root_uint16_t vrow = 0; vrow < con->scroll_height; vrow++)
    con->fillvrow (con, vrow, bg);
  con->x = 0;
  con->y = 0;
  con->offset = 0;
  console_draw (con);
  console_sync_cursor (con);
}

void
console_offsetup (root_console_t *con)
{
  if (con->offset)
    {
      con->offset--;
      console_draw (con);
    }
}

void
console_offsetdown (root_console_t *con)
{
  if (con->offset + con->height + 1 <= con->scroll_height)
    {
      con->offset++;
      console_draw (con);
    }
}

root_ssize_t
_console_read (root_fd_t *fd, char *buf, root_size_t sz)
{
  root_console_fd_t *con_fd = (root_console_fd_t *) fd;
  root_console_t *con = con_fd->con;
  root_input_t input;
  if (con->in.buf == NULL)
    {
      con->in.buf = root_alloc_pages (1);
      if (con->in.buf == NULL)
        return -1;
      con->in.cap = ROOT_PAGE_SIZE;
    }
  if (con->in.sz)
    {
      if (sz >= con->in.sz)
        sz = con->in.sz;
      root_memcpy (buf, con->in.buf, sz);
      con->in.sz -= sz;
      return sz;
    }
  while (1)
    {
      if (root_poll_input (&input) && input.state != ROOT_KEY_STATE_RELEASE)
        {
          switch (input.keycode)
            {
            case ROOT_KEY_TAB:
              break;
            case ROOT_KEY_BACKSPACE:
              if (con->in.cursor)
                {
                  root_uint16_t oldx, oldy;
                  root_size_t cursor;
                  char *pos = con->in.buf + con->in.cursor;
                  con->putchar (con, '\b');
                  oldx = con->x;
                  oldy = con->y;
                  root_memmove (pos - 1, pos, con->in.sz - con->in.cursor);
                  cursor = --con->in.cursor;
                  --con->in.sz;
                  for (; cursor < con->in.sz; cursor++)
                    con->putchar (con, con->in.buf[cursor]);
                  con->putchar (con, ' ');
                  con->moveto (con, oldx, oldy);
                }
              break;
            case ROOT_KEY_ENTER:
              con->putchar (con, '\n');
              con->in.cursor = 0;
              if (sz >= con->in.sz)
                sz = con->in.sz;
              root_memcpy (buf, con->in.buf, sz);
              con->in.sz -= sz;
              return sz;
            case ROOT_KEY_LEFT:
              if (con->in.cursor)
                {
                  con->putchar (con, '\b');
                  con->in.cursor--;
                }
              break;
            case ROOT_KEY_RIGHT:
              if (con->in.cursor < con->in.sz)
                {
                  con->advance (con);
                  con->in.cursor++;
                }
              break;
            case ROOT_KEY_PAGEUP:
              console_offsetup (con);
              break;
            case ROOT_KEY_PAGEDOWN:
              console_offsetdown (con);
              break;
            default:
              if (input.ascii && con->in.sz + 1 < con->in.cap)
                {
                  root_uint16_t oldx = con->x, oldy = con->y;
                  root_size_t cursor = con->in.cursor;
                  char *pos = con->in.buf + con->in.cursor;
                  root_memmove (pos + 1, pos, con->in.sz - con->in.cursor);
                  con->in.buf[con->in.cursor++] = input.ascii;
                  con->in.sz++;
                  for (; cursor < con->in.sz; cursor++)
                    con->putchar (con, con->in.buf[cursor]);
                  con->moveto (con, oldx, oldy);
                  con->advance (con);
                }
            }
        }
      /* scheduling is necessary since we block */
      root_schedule_tasks ();
    }
}

root_ssize_t
_console_write (root_fd_t *fd, const char *buf, root_size_t sz)
{
  root_console_fd_t *con_fd = (root_console_fd_t *) fd;
  root_console_t *con = con_fd->con;
  for (root_size_t i = 0; i < sz; i++)
    con->putchar (con, buf[i]);
  return sz;
}
