#include "../cpu/idt.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"
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
    // 1. Request a 4-byte slice from the Heap Allocator
    u32* heap_test = (u32*)kmalloc(sizeof(u32));

    // 2. Hardware Safety Check: Ensure the pointer is valid (not 0)
    if (heap_test != 0) {
        
        // 3. Write a persistent hexadecimal signature to the allocated RAM
        *heap_test = 0xDEADBEEF;

        // 4. Read the signature back to verify the RAM retained the exact data
        if (*heap_test == 0xDEADBEEF) {
            
            // 5. Visual Telemetry: Render a 50x50 Purple block (Hex: 0x800080)
            // This manipulates the global_framebuffer array directly.
            // Replace '1024' with your actual screen width if your VESA mode differs.
            for (u32 y = 20; y < 70; y++) {
                for (u32 x = 20; x < 70; x++) {
                    global_framebuffer[y * 1024 + x] = 0x800080; 
                }
            }
        }

        // 6. Release the block back to the pool to prevent memory leaks
        kfree(heap_test);
    }
    // CPU execution idles here.
    while(1) { __asm__("hlt"); }
}