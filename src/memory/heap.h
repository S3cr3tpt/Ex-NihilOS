#ifndef HEAP_H
#define HEAP_H

#include "../global/types.h"

// The compiler will naturally align this to 16 bytes to prevent hardware faults.
typedef struct heap_block {
    u32 size;               
    u8 is_free;             
    struct heap_block* next; 
} heap_block_t;

void heap_init();
void* kmalloc(u32 size);
void kfree(void* ptr);

#endif