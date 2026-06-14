#ifndef PMM_H
#define PMM_H

#include "../global/types.h"

__attribute__((packed)) struct e820_entry {
    u64 base;      
    u64 length;    
    u32 type;      // 1 = Usable RAM
    u32 acpi;      
};

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8

void pmm_init();
void* pmm_alloc_block();

#endif