#ifndef ROOT_VGA_H
#define ROOT_VGA_H 1

#include "console/console.h"

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

typedef struct root_vga_console_t
{
  root_console_t base;
  root_uint16_t max_scanline;
  root_uint8_t *fb;
} root_vga_console_t;

root_err_t vga_console_init (root_vga_console_t *con);

void vga_putglyph (struct root_console_t *con, char ch, root_uint16_t x,
                   root_uint16_t y);
void vga_putvrow (struct root_console_t *con, root_uint16_t vrow,
                  root_uint16_t row);
void vga_fillvrow (struct root_console_t *con, root_uint16_t vrow,
                   root_uint32_t bg);
void vga_putcursor (struct root_console_t *con, root_uint16_t x,
                    root_uint16_t y);
void vga_setcursor (struct root_console_t *con, root_uint8_t enabled);
void vga_blinkcursor (struct root_console_t *con);

void vga_scroll (struct root_console_t *con);

#endif
