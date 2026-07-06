#include "../cpu/idt.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"
#include "../cpu/pic.h"
#include "../cpu/io.h"
#include "../global/types.h"
#include "shell.h"

u32* global_framebuffer;

void kernel_main(u32* framebuffer) {
    // 1. Establish Display Link
    global_framebuffer = framebuffer; 
    
    // 2. Arm Core Infrastructure
    idt_install();
    pmm_init(); 
    vmm_init(); 
    heap_init(); 

    // 3. Configure Hardware Pipeline
    pic_remap(0x20, 0x28);
    outb(0x21, 0xFF); 
    outb(0xA1, 0xFF); 
    pic_clear_mask(1); // Keyboard Unmask

    // 4. Ignite Tiling UI & Software Shell
    shell_init();

    // 5. Drop Shields & Halt
    __asm__ volatile ("sti");
    while(1) { __asm__ volatile ("hlt"); }
}