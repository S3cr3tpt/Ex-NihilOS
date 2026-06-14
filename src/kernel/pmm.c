#include "pmm.h"

u8* memory_bitmap;
u64 total_memory_size = 0;
u64 max_blocks = 0;
u64 used_blocks = 0;

static inline void bitmap_set(u64 bit) {
    memory_bitmap[bit / BLOCKS_PER_BYTE] |= (1 << (bit % BLOCKS_PER_BYTE));
}

static inline void bitmap_clear(u64 bit) {
    memory_bitmap[bit / BLOCKS_PER_BYTE] &= ~(1 << (bit % BLOCKS_PER_BYTE));
}
// Helper: Test if a specific bit is 1 (Occupied) or 0 (Free)
static inline int bitmap_test(u64 bit) {
    return memory_bitmap[bit / BLOCKS_PER_BYTE] & (1 << (bit % BLOCKS_PER_BYTE));
}

void pmm_init() {
    u32* entry_count_ptr = (u32*)0x5000;
    u32 entry_count = *entry_count_ptr;
    struct e820_entry* map = (struct e820_entry*)0x5004;

    // Scan for highest usable memory address
    for (u32 i = 0; i < entry_count; i++) {
        if (map[i].type == 1) { 
            u64 region_top = map[i].base + map[i].length;
            if (region_top > total_memory_size) {
                total_memory_size = region_top;
            }
        }
    }

    max_blocks = total_memory_size / PAGE_SIZE;

    // Place bitmap safely at 2MB mark
    memory_bitmap = (u8*)0x200000;
    u64 bitmap_size = max_blocks / BLOCKS_PER_BYTE;

    // Lock entire matrix
    for (u64 i = 0; i < bitmap_size; i++) {
        memory_bitmap[i] = 0xFF; 
    }
    used_blocks = max_blocks;

    // Unlock verified usable frames
    for (u32 i = 0; i < entry_count; i++) {
        if (map[i].type == 1) { 
            u64 start_block = map[i].base / PAGE_SIZE;
            u64 block_count = map[i].length / PAGE_SIZE;
            
            for (u64 b = 0; b < block_count; b++) {
                bitmap_clear(start_block + b);
                used_blocks--;
            }
        }
    }

    // Re-lock the first 3MB (Kernel + Bootloader + Bitmap)
    for (u64 i = 0; i < (0x300000 / PAGE_SIZE); i++) {
        bitmap_set(i);
        used_blocks++;
    }
}
// Scan the bitmap and allocate the first free 4KB block
void* pmm_alloc_block() {
    for (u64 i = 0; i < max_blocks; i++) {
        if (!bitmap_test(i)) {     // If the bit is 0 (Free)
            bitmap_set(i);         // Lock it (Set to 1)
            used_blocks++;
            
            // Calculate and return the absolute physical address
            u64 physical_address = i * PAGE_SIZE;
            return (void*)physical_address; 
        }
    }
    return 0; // System Crash: Out of Memory
}