#ifndef ROOT_VBE_H
#define ROOT_VBE_H 1

#include "types.h"

typedef struct root_video_mode_t
{
  root_u16 attributes;
  root_u8 window_a;
  root_u8 window_b;
  root_u16 granularity;
  root_u16 window_size;
  root_u16 segment_a;
  root_u16 segment_b;
  root_u32 win_func_ptr;
  root_u16 pitch;
  root_u16 width;
  root_u16 height;
  root_u8 w_char;
  root_u8 y_char;
  root_u8 planes;
  root_u8 bpp;
  root_u8 banks;
  root_u8 memory_model;
  root_u8 bank_size;
  root_u8 image_pages;
  root_u8 reserved0;
  root_u8 red_mask;
  root_u8 red_pos;
  root_u8 green_mask;
  root_u8 green_pos;
  root_u8 blue_mask;
  root_u8 blue_pos;
  root_u8 reserved_mask;
  root_u8 reserved_pos;
  root_u8 color_attributes;
  root_u8 *framebuffer;
  root_u32 off_screen_mem_off;
  root_u32 off_screen_mem_size;
  root_u8 reserved1[206];
} __attribute__ ((packed)) root_video_mode_t;

int root_vbe_find_best_videomode (root_u16 width, root_u16 height,
                                  root_u16 depth,
                                  root_video_mode_t *video_mode);

void root_vbe_set_videomode (root_u16 mode);

#endif
