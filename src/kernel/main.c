#include "../cpu/idt.h"
#include "pmm.h"
#include "../global/types.h"

extern u64 total_memory_size;
extern void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color);
extern void print_hex_64(int start_x, int start_y, u64 value, u32 color);

u32* global_framebuffer;

void kernel_main(u32* framebuffer) {
    global_framebuffer = framebuffer; 
    
    idt_install();
    pmm_init(); 

    // 1. Print total RAM in Cyan
    print_memory_string(100, 100, total_memory_size, 0x0000FFFF); 

    // 2. ACTIVE TEST: Request one 4KB frame of memory
    void* new_block = pmm_alloc_block();

    // 3. Print the Hexadecimal address of the new block in Red
    print_hex_64(100, 150, (u64)new_block, 0x00FF0000);

    while(1) { __asm__("hlt"); }
}