#ifndef ROOT_VGA_H
#define ROOT_VGA_H 1

#include "text.h"

#define ROOT_VGA_COLOR_BLACK        0x0
#define ROOT_VGA_COLOR_BLUE         0x1
#define ROOT_VGA_COLOR_GREEN        0x2
#define ROOT_VGA_COLOR_CYAN         0x3
#define ROOT_VGA_COLOR_RED          0x4
#define ROOT_VGA_COLOR_PURPLE       0x5
#define ROOT_VGA_COLOR_BROWN        0x6
#define ROOT_VGA_COLOR_GRAY         0x7
#define ROOT_VGA_COLOR_DARK_GRAY    0x8
#define ROOT_VGA_COLOR_LIGHT_BLUE   0x9
#define ROOT_VGA_COLOR_LIGHT_GREEN  0xA
#define ROOT_VGA_COLOR_LIGHT_CYAN   0xB
#define ROOT_VGA_COLOR_LIGHT_RED    0xC
#define ROOT_VGA_COLOR_LIGHT_PURPLE 0xD
#define ROOT_VGA_COLOR_YELLOW       0xE
#define ROOT_VGA_COLOR_WHITE        0xF

typedef struct root_vga_state_t
{
  root_u16 width, height, stride;
  root_u8 *data, max_scanline;
} root_vga_state_t;

typedef struct root_vga_term_t
{
  root_term_t base;
  root_vga_state_t state;
  root_u8 cursor_color;
} root_vga_term_t;

int root_initvga (root_vga_term_t *out);

#endif
