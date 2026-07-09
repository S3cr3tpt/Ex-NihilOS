#include "../cpu/idt.h"
#include "../memory/pmm.h"
#include "../memory/vmm.h"
#include "../memory/heap.h"
#include "../cpu/pic.h"
#include "../cpu/io.h"
#include "../global/types.h"
#include "../fs/fs.h"
#include "shell.h"

u32* global_framebuffer;

void kernel_main(u32* framebuffer) {
    global_framebuffer = framebuffer; 
    
    idt_install();
    pmm_init(); 
    vmm_init(); 
    heap_init(); 

    pic_remap(0x20, 0x28);
    outb(0x21, 0xFF); 
    outb(0xA1, 0xFF); 
    pic_clear_mask(1); 

    // 1. Evaluate and mount persistent storage
    fs_init();

    // 2. Ignite TUI
    shell_init();

    __asm__ volatile ("sti");
    while(1) { __asm__ volatile ("hlt"); }
}