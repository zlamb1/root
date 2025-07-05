#ifndef ROOT_GFX_MODE_H
#define ROOT_GFX_MODE_H 1

#include "types.h"

// TODO: use masks
typedef struct root_gfx_mode_t
{
  root_u16 width, height;
  root_size_t stride;
  root_size_t bpp; // bytes per pixel
  root_u8 *fb;
} root_gfx_mode_t;

#endif
