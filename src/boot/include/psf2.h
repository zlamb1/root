#ifndef ROOT_PSF2_H
#define ROOT_PSF2_H

#include "types.h"

unsigned char *root_psf2_getglyph (root_u8 ch);

void root_psf2_putglyph (void *data, root_u8 ch, root_u32 fg, root_u32 bg,
                         root_size_t pitch);

#endif
