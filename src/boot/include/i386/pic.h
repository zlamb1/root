#ifndef ROOT_PIC_H
#define ROOT_PIC_H 1

#include "types.h"

void root_pic_init (root_uint8_t pic1_offset, root_uint8_t pic2_offset);
void root_pic_mask_irq (root_uint8_t irq);
void root_pic_unmask_irq (root_uint8_t irq);
void root_pic_eoi (root_uint8_t irq);
void root_pic_disable (void);

#endif
