#include "../global/types.h"

#ifndef IDT_H
#define IDT_H



// 1. The 16-Byte Gate Structure (Hardware Specification)
__attribute__((packed)) struct idt_entry {
    uint16_t isr_low;      // Lower 16 bits of ISR address
    uint16_t kernel_cs;    // Code segment selector (0x08)
    uint8_t  ist;          // Interrupt Stack Table offset (set to 0)
    uint8_t  attributes;   // Type and attributes (0x8E for 64-bit interrupt gate)
    uint16_t isr_mid;      // Middle 16 bits of ISR address
    uint32_t isr_high;     // Upper 32 bits of ISR address
    uint32_t reserved;     // Set to 0
};

// 2. The IDT Pointer (What we feed to the `lidt` command)
__attribute__((packed)) struct idtr {
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

// 3. The Execution Declarations
void idt_set_gate(int n, uint64_t handler);
void idt_install();

// External Assembly stubs we generated in the macro
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14(); 

#endif