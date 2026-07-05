#include "../cpu/idt.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"
#include "../cpu/pic.h"
#include "../cpu/io.h"
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
    heap_init(); // MANDATORY: Required before kmalloc

    print_memory_string(100, 100, total_memory_size, 0x0000FFFF); 

    void* new_physical_block = pmm_alloc_block();
    print_hex_64(100, 150, (u64)new_physical_block, 0x00FF0000);

    void* virtual_target = (void*)0x0000000077770000; 
    vmm_map_page(virtual_target, new_physical_block, PTE_PRESENT | PTE_RW);
    volatile u64* v_ptr = (volatile u64*)virtual_target;
    *v_ptr = 0xC0DEBABE; 
    print_hex_64(100, 200, *v_ptr, 0x0000FF00);

    u32* heap_test = (u32*)kmalloc(sizeof(u32));
    if (heap_test != 0) {
        *heap_test = 0xDEADBEEF;
        if (*heap_test == 0xDEADBEEF) {
            // Updated to 1920 width to match your VESA mode
            for (u32 y = 20; y < 70; y++) {
                for (u32 x = 20; x < 70; x++) {
                    global_framebuffer[y * 1920 + x] = 0x800080; 
                }
            }
        }
        kfree(heap_test);
    }

    // ------------------------------------------------
    // HARDWARE PIPELINE (PREVENTS TRIPLE FAULT)
    // ------------------------------------------------
    
    // 1. Remap the PIC routing out of the Exception Zone
    pic_remap(0x20, 0x28);

    // 2. Deafen both PICs (Mask all lines, silencing the BIOS timer)
    outb(0x21, 0xFF); 
    outb(0xA1, 0xFF); 

    // 3. Unmask ONLY the Keyboard (IRQ 1)
    pic_clear_mask(1);

    // 4. Drop CPU shields
    __asm__ volatile ("sti");

    // 5. Idle Execution
    while(1) { __asm__ volatile ("hlt"); }
}