#ifndef ROOT_TEXT_H
#define ROOT_TEXT_H 1

#include "types.h"

#define ROOT_TEXT_POS_RESET 0xFFFF

typedef struct root_text_pos_t
{
  root_u16 x, y;
} root_text_pos_t;

typedef struct root_text_cursor_t
{
  root_u16 x, y;
  root_u8 visible;
} root_text_cursor_t;

typedef enum root_term_type_t
{
  ROOT_TERM_TYPE_VGA,
  ROOT_TERM_TYPE_GFX
} root_term_type_t;

typedef enum root_term_scroll_type_t
{
  ROOT_SCROLL_TYPE_WRAP,
  ROOT_SCROLL_TYPE_NEWLINE
} root_term_scroll_type_t;

typedef struct root_term_attribs_t
{
  root_u16 tabsize;
  root_term_scroll_type_t scrolltype;
} root_term_attribs_t;

typedef struct root_term_t
{
  root_term_type_t type;
  root_term_attribs_t attribs;
  root_text_pos_t pos;
  root_text_cursor_t cursor;

  root_u16 (*getwidth) (struct root_term_t *term);
  root_u16 (*getheight) (struct root_term_t *term);

  void (*putchar) (struct root_term_t *term, char c);
  void (*putchar_unsynced) (struct root_term_t *term, char c);
  void (*advance) (struct root_term_t *term, root_u16 x, root_u16 y);
  void (*setcursorpos) (struct root_term_t *term, root_u16 x, root_u16 y);
  void (*setcursorvisible) (struct root_term_t *term, root_u8 visible);
  void (*setcursorcolor) (struct root_term_t *term, root_u32 fg, root_u32 bg);
  void (*clear) (struct root_term_t *term, root_u32 color);
} root_term_t;

int root_terminit (root_term_t *term);

#endif
