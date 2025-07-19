#ifndef ROOT_TERM_H
#define ROOT_TERM_H

#include "file.h"
#include "types.h"

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

typedef enum
{
  ROOT_TERM_STATE_WRI,
  ROOT_TERM_STATE_ESC,
  ROOT_TERM_STATE_CSI
} root_term_state_t;

typedef struct root_term_t
{
  void (*init) (struct root_term_t *term);
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
  struct root_term_t *next;
} root_term_t;

void root_term_init (root_term_t *term);

void root_register_term (root_term_t *term);
root_err_t root_unregister_term (root_term_t *term);
root_term_t *root_iterate_terms (root_term_t *term);

void root_term_set_primary (root_term_t *term);

void root_term_putchar (root_term_t *term, char ch);

root_ssize_t root_term_write (root_file_t *file, const char *buf,
                              root_size_t size);

int root_term_ioctl (root_file_t *file, int op, va_list args);

#endif
