#ifndef ROOT_VGA_H
#define ROOT_VGA_H 1

#include "kern/mod.h"
#include "kern/term.h"

extern root_initmod_t root_vga_term_initmod;

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
