#include "idt.h"

// The absolute memory array
struct idt_entry idt[256];
struct idtr idtp;

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

// The Triage Function (Called by your Assembly `isr_common_stub`)
void isr_handler(uint64_t* stack_frame) {
    // stack_frame[0] is the error code (or our dummy 0)
    // stack_frame[1] is the interrupt number pushed by the macro
    
    int int_no = stack_frame[1];
    
    // DEBUG: We simply freeze the CPU and wait for visual confirmation
    while(1) { __asm__("hlt"); }
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    // Zero out the array before writing
    uint8_t *idt_ptr = (uint8_t *)&idt;
    for (int i = 0; i < sizeof(struct idt_entry) * 256; i++) {
        idt_ptr[i] = 0;
    }

    // Map the blast doors
    idt_set_gate(0, (uint64_t)isr0);
    idt_set_gate(1, (uint64_t)isr1);
    // ... map up to 31 for CPU panics
    idt_set_gate(14, (uint64_t)isr14); 

    // Lock the matrix into the CPU
    __asm__ volatile ("lidt %0" : : "m" (idtp));
}