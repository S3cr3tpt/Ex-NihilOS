#include "idt.h"
#include "../global/types.h"

// The absolute memory array
struct idt_entry idt[256];
struct idtr idtp;

extern u32* global_framebuffer;

// Function to calculate and inject the 64-bit memory address into the 16-byte gate
void idt_set_gate(int n, uint64_t handler) {
    idt[n].isr_low = (uint16_t)handler;
    idt[n].kernel_cs = 0x18; // Your 64-bit code segment from GDT
    idt[n].ist = 0;
    idt[n].attributes = 0x8E; // Present, Ring 0, 64-bit Interrupt Gate
    idt[n].isr_mid = (uint16_t)(handler >> 16);
    idt[n].isr_high = (uint32_t)(handler >> 32);
    idt[n].reserved = 0;
}

// The Triage Function (Called by your AssemblyD `isr_common_stub`)
void isr_handler(uint64_t* stack_frame) {
    // stack_frame[0] is the error code (or our dummy 0)
    // stack_frame[1] is the interrupt number pushed by the macro
    int width = 1920; 
    int height = 1080;
    int int_no = stack_frame[1];
        switch (int_no) {
        case 0:
            u64 offset = width * (1080/2) + 1920/2; // Center of the screen
            global_framebuffer[offset] = 0x0000FF00; 
            while(1) { __asm__("hlt");}

            break;
        
        default:
            for (int y=0; y < height; y++) {
                for (int x=0; x < width; x++) {
                    u64 offset = (y * width) + x;
                    global_framebuffer[offset] = 0x00FF0000; 
                }
            }
            while(1) { __asm__("hlt");}
            break;
        }
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    // Zero out the array before writing
    uint8_t *idt_ptr = (uint8_t *)&idt;
    for (int i = 0; i < sizeof(struct idt_entry) * 256; i++) {
        idt_ptr[i] = 0;
    }

    idt_set_gate(0, (uint64_t)isr0);
    idt_set_gate(1, (uint64_t)isr1);
    idt_set_gate(2, (uint64_t)isr2);
    idt_set_gate(3, (uint64_t)isr3);
    idt_set_gate(4, (uint64_t)isr4);
    idt_set_gate(5, (uint64_t)isr5);
    idt_set_gate(6, (uint64_t)isr6);
    idt_set_gate(7, (uint64_t)isr7);
    idt_set_gate(8, (uint64_t)isr8);
    idt_set_gate(9, (uint64_t)isr9);
    idt_set_gate(10, (uint64_t)isr10);
    idt_set_gate(11, (uint64_t)isr11);
    idt_set_gate(12, (uint64_t)isr12);
    idt_set_gate(13, (uint64_t)isr13);
    idt_set_gate(14, (uint64_t)isr14); 

    // Lock the matrix into the CPU
    __asm__ volatile ("lidt %0" : : "m" (idtp));
}