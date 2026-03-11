#include "../cpu/idt.h" // Route to the new Ring 0 directory

typedef unsigned long long u64;
typedef unsigned int u32;

void kernel_main(u32* framebuffer) {
    // 1. Lock the IDT into the CPU
    idt_install();

    // 2. Draw your baseline diagnostic (Green Box)
    int width = 1920; 
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            u64 offset = (y * width) + x;
            framebuffer[offset] = 0x0000FF00; 
        }
    }
    
    // 3. THE TRIGGER: Intentionally detonate a Divide-by-Zero hardware panic
    __asm__ volatile ("int $0");

    // The CPU should never reach this loop if the interrupt fires
    while(1) { __asm__("hlt"); }
}