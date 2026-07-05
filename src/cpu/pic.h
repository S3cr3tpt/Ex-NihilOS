#ifndef PIC_H
#define PIC_H

#include "../global/types.h"

void pic_remap(u32 offset1, u32 offset2);
void pic_set_mask(u8 irq_line);
void pic_clear_mask(u8 irq_line);

#endif