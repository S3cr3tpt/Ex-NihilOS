#include "../cpu/idt.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../global/types.h"

extern u64 total_memory_size;
extern void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color);
extern void print_hex_64(int start_x, int start_y, u64 value, u32 color);

u32* global_framebuffer;

void kernel_main(u32* framebuffer) {
    global_framebuffer = framebuffer; 
    
    idt_install();
    pmm_init(); 
    vmm_init(); 

    // 1. UI: Print Total RAM (Cyan)
    print_memory_string(100, 100, total_memory_size, 0x0000FFFF); 

    // 2. PMM: Print the first raw physical allocation (Red)
    void* new_physical_block = pmm_alloc_block();
    print_hex_64(100, 150, (u64)new_physical_block, 0x00FF0000);

    // 3. VMM POSITIVE TEST: The Routing Verification
    // Pick an arbitrary, unmapped virtual address
    void* virtual_target = (void*)0x0000000077770000; 

    // Command the Matrix to link virtual_target to new_physical_block
    vmm_map_page(virtual_target, new_physical_block, PTE_PRESENT | PTE_RW);

    // Write a signature to the virtual address
    volatile u64* v_ptr = (volatile u64*)virtual_target;
    *v_ptr = 0xC0DEBABE; 

    // Read the signature back from the virtual address and print it (Green)
    print_hex_64(100, 200, *v_ptr, 0x0000FF00);

    // CPU execution idles here.
    while(1) { __asm__("hlt"); }
}