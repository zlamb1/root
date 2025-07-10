#ifndef ROOT_VBE_H
#define ROOT_VBE_H 1

#include "types.h"

typedef struct root_video_mode_t
{
  root_uint16_t attributes;
  root_uint8_t window_a;
  root_uint8_t window_b;
  root_uint16_t granularity;
  root_uint16_t window_size;
  root_uint16_t segment_a;
  root_uint16_t segment_b;
  root_uint32_t win_func_ptr;
  root_uint16_t pitch;
  root_uint16_t width;
  root_uint16_t height;
  root_uint8_t w_char;
  root_uint8_t y_char;
  root_uint8_t planes;
  root_uint8_t bpp;
  root_uint8_t banks;
  root_uint8_t memory_model;
  root_uint8_t bank_size;
  root_uint8_t image_pages;
  root_uint8_t reserved0;
  root_uint8_t red_mask;
  root_uint8_t red_pos;
  root_uint8_t green_mask;
  root_uint8_t green_pos;
  root_uint8_t blue_mask;
  root_uint8_t blue_pos;
  root_uint8_t reserved_mask;
  root_uint8_t reserved_pos;
  root_uint8_t color_attributes;
  root_uint8_t *framebuffer;
  root_uint32_t off_screen_mem_off;
  root_uint32_t off_screen_mem_size;
  root_uint8_t reserved1[206];
} __attribute__ ((packed)) root_video_mode_t;

int root_vbe_find_best_videomode (root_uint16_t width, root_uint16_t height,
                                  root_uint16_t depth,
                                  root_video_mode_t *video_mode);

void root_vbe_set_videomode (root_uint16_t mode);

#endif
