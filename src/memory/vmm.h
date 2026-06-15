#ifndef VMM_H
#define VMM_H

#include "../global/types.h"

// Hardware Execution Flags
#define PTE_PRESENT  0x01
#define PTE_RW       0x02
#define PTE_USER     0x04

// 512 entries * 8 bytes = 4096 bytes (Exactly 1 Physical Frame)
struct page_table {
    u64 entries[512];
} __attribute__((aligned(4096)));

void vmm_init();
void vmm_map_page(void* virtual_address, void* physical_address, u32 flags);

#endif