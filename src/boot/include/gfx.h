#ifndef ROOT_GFX_H
#define ROOT_GFX_H 1

#include "mode.h"
#include "text.h"

typedef struct root_gfx_term_t
{
  root_term_t base;
  root_video_mode_t video_mode;
  root_u32 cursor_fg, cursor_bg;
} root_gfx_term_t;

int root_initgfx_term (root_gfx_term_t *term, root_video_mode_t *video_mode);

#endif
