#ifndef ROOT_VGA_H
#define ROOT_VGA_H 1

#include "term.h"

#define VGA_COLOR_BLACK        0x0
#define VGA_COLOR_BLUE         0x1
#define VGA_COLOR_GREEN        0x2
#define VGA_COLOR_CYAN         0x3
#define VGA_COLOR_RED          0x4
#define VGA_COLOR_PURPLE       0x5
#define VGA_COLOR_BROWN        0x6
#define VGA_COLOR_GRAY         0x7
#define VGA_COLOR_DARK_GRAY    0x8
#define VGA_COLOR_LIGHT_BLUE   0x9
#define VGA_COLOR_LIGHT_GREEN  0xA
#define VGA_COLOR_LIGHT_CYAN   0xB
#define VGA_COLOR_LIGHT_RED    0xC
#define VGA_COLOR_LIGHT_PURPLE 0xD
#define VGA_COLOR_YELLOW       0xE
#define VGA_COLOR_WHITE        0xF

void vga_init (root_term_t *term);
void vga_putchar (root_term_t *term, char ch);
void vga_putcursor (root_term_t *term, root_uint16_t x, root_uint16_t y);
root_uint32_t vga_getfg (root_term_t *term);
void vga_setfg (root_term_t *term, root_uint32_t fg);
root_uint32_t vga_getbg (root_term_t *term);
void vga_setbg (root_term_t *term, root_uint32_t bg);
root_term_size_t vga_getwh (root_term_t *term);
root_term_pos_t vga_getxy (root_term_t *term);
void vga_setxy (root_term_t *term, root_uint16_t x, root_uint16_t y);
void vga_advance (root_term_t *term);
void vga_newline (struct root_term_t *term);
void vga_clr (root_term_t *term);

void root_vga_term_mod_init (void);
void root_vga_term_mod_fini (void);

#endif
