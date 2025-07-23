#ifndef ROOT_TERM_H
#define ROOT_TERM_H

#include "file.h"
#include "types.h"

#define ROOT_TERM_COLOR_BLACK        0x0
#define ROOT_TERM_COLOR_BLUE         0x1
#define ROOT_TERM_COLOR_GREEN        0x2
#define ROOT_TERM_COLOR_CYAN         0x3
#define ROOT_TERM_COLOR_RED          0x4
#define ROOT_TERM_COLOR_PURPLE       0x5
#define ROOT_TERM_COLOR_BROWN        0x6
#define ROOT_TERM_COLOR_GRAY         0x7
#define ROOT_TERM_COLOR_DARK_GRAY    0x8
#define ROOT_TERM_COLOR_LIGHT_BLUE   0x9
#define ROOT_TERM_COLOR_LIGHT_GREEN  0xA
#define ROOT_TERM_COLOR_LIGHT_CYAN   0xB
#define ROOT_TERM_COLOR_LIGHT_RED    0xC
#define ROOT_TERM_COLOR_LIGHT_PURPLE 0xD
#define ROOT_TERM_COLOR_YELLOW       0xE
#define ROOT_TERM_COLOR_WHITE        0xF

#define ROOT_TERM_IOCTL_SYNC_CURSOR 0

typedef struct
{
  root_uint16_t x, y;
} root_term_pos_t;

typedef struct
{
  root_uint16_t width, height;
} root_term_size_t;

typedef struct
{
  root_file_t base;
  struct root_term_t *term;
} root_term_file_t;

typedef struct
{
  root_size_t cap, cnt;
  union
  {
    int i;
    const char *str;
  } *buf;
} root_term_args_t;

typedef struct root_term_t
{
  void (*putchar) (struct root_term_t *term, char ch);
  void (*putcursor) (struct root_term_t *term, root_uint16_t x,
                     root_uint16_t y);
  root_uint32_t (*getfg) (struct root_term_t *term);
  void (*setfg) (struct root_term_t *term, root_uint32_t fg);
  root_uint32_t (*getbg) (struct root_term_t *term);
  void (*setbg) (struct root_term_t *term, root_uint32_t bg);
  root_term_size_t (*getwh) (struct root_term_t *term);
  root_term_pos_t (*getxy) (struct root_term_t *term);
  void (*setxy) (struct root_term_t *term, root_uint16_t x, root_uint16_t y);
  void (*advance) (struct root_term_t *term);
  void (*newline) (struct root_term_t *term);
  void (*clr) (struct root_term_t *term);
  root_term_file_t stdout;

  unsigned char state;
  root_term_args_t args;
  root_term_pos_t saved_cursor_pos;

  struct root_term_t *next;
} root_term_t;

void root_term_init (root_term_t *term);

void root_register_term (root_term_t *term);
root_err_t root_unregister_term (root_term_t *term);
root_term_t *root_iterate_terms (root_term_t *term);

void root_term_set_primary (root_term_t *term);

void root_term_putchar (root_term_t *term, char ch);

void root_term_sync_cursor (root_term_t *term);

root_ssize_t root_term_write (root_file_t *file, const char *buf,
                              root_size_t size);

int root_term_ioctl (root_file_t *file, int op, va_list args);

#endif
