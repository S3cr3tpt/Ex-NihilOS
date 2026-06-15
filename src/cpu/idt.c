#include "idt.h"
#include "../global/types.h"

extern u32* global_framebuffer;
extern void print_hex_64(int start_x, int start_y, u64 value, u32 color);

struct idt_entry idt[256];
struct idtr idtp;

void idt_set_gate(int n, uint64_t handler) {
    idt[n].isr_low = (uint16_t)handler;
    idt[n].kernel_cs = 0x18; 
    idt[n].ist = 0;
    idt[n].attributes = 0x8E; 
    idt[n].isr_mid = (uint16_t)(handler >> 16);
    idt[n].isr_high = (uint32_t)(handler >> 32);
    idt[n].reserved = 0;
}

void isr_handler(uint64_t* stack_frame) {
    int width = 1920; 
    int height = 1080;
    
    int int_no = stack_frame[15]; 
    
    switch (int_no) {
        case 0:
            global_framebuffer[(width * (1080/2)) + (1920/2)] = 0x0000FF00; 
            while(1) { __asm__("hlt");}
            break;
            
        case 14: { // INTERRUPT 14: PAGE FAULT
            u64 faulting_address;
            // CR2 holds the exact virtual address that caused the crash
            __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
            
            // Print the malicious address in Red at coordinates X:100, Y:200
            print_hex_64(100, 200, faulting_address, 0x00FF0000);
            
            while(1) { __asm__("hlt");}
            break;
        }
        
        default:
            for (int y=0; y < height; y++) {
                for (int x=0; x < width; x++) {
                    global_framebuffer[(y * width) + x] = 0x00FF0000; 
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