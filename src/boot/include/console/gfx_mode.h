#ifndef ROOT_GFX_MODE_H
#define ROOT_GFX_MODE_H 1

#include "types.h"

// TODO: use masks
typedef struct root_gfx_mode_t
{
  root_uint16_t width, height;
  root_size_t stride;
  root_size_t bpp; // bytes per pixel
  root_uint8_t *fb;
} root_gfx_mode_t;

#endif
