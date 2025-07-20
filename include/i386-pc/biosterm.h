#ifndef ROOT_BIOS_TERM_H
#define ROOT_BIOS_TERM_H 1

#include "kern/term.h"

void root_init_bios_term (void);

void bios_term_putchar (root_term_t *t, char ch);
void bios_term_putcursor (root_term_t *t, root_uint16_t x, root_uint16_t y);
root_uint32_t bios_term_getfg (root_term_t *t);
void bios_term_setfg (root_term_t *t, root_uint32_t fg);
root_uint32_t bios_term_getbg (root_term_t *t);
void bios_term_setbg (root_term_t *t, root_uint32_t bg);
root_term_size_t bios_term_getwh (root_term_t *t);
root_term_pos_t bios_term_getxy (root_term_t *t);
void bios_term_setxy (root_term_t *t, root_uint16_t x, root_uint16_t y);
void bios_term_advance (root_term_t *t);
void bios_term_newline (root_term_t *t);
void bios_term_clr (root_term_t *t);

#endif
