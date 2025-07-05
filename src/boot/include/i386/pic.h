#ifndef ROOT_PIC_H
#define ROOT_PIC_H 1

#include "types.h"

void root_pic_init (root_u8 pic1_offset, root_u8 pic2_offset);
void root_pic_mask_irq (root_u8 irq);
void root_pic_unmask_irq (root_u8 irq);
void root_pic_eoi (root_u8 irq);
void root_pic_disable (void);

#endif
