// src/kernel/main.c
typedef unsigned long long u64;
typedef unsigned int u32;

void kernel_main(u32* framebuffer) {
    // BACK TO HD
    int width = 1920; 
    
    // Draw a 100x100 Green Box
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            
            u64 offset = (y * width) + x;
            framebuffer[offset] = 0x0000FF00; 
        }
    }
    
    while(1) { __asm__("hlt"); }
}