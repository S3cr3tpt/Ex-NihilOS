#include "idt.h"
#include "io.h"
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

    // ==========================================
    // HARDWARE INTERRUPT SECTOR (Vectors 32-47)
    // ==========================================
    if (int_no >= 32 && int_no <= 47) {
        
     // IRQ 1: KEYBOARD INTERCEPT
        if (int_no == 33) {
            u8 scancode = inb(0x60); // 1. Drain the silicon buffer to allow future keystrokes
            
            // 2. Visual Telemetry: Render 50x50 Grey block (Hex: 0x808080)
            // Offset X axis to 80-130 to position it directly right of the Purple block
            for (u32 y = 20; y < 70; y++) {
                for (u32 x = 80; x < 130; x++) {
                    global_framebuffer[y * width + x] = 0x808080; 
                }
            }
        }

        // Blast EOI (End of Interrupt) to hardware
        if (int_no >= 40) {
            outb(0xA0, 0x20); // Slave PIC
        }
        outb(0x20, 0x20);     // Master PIC
        return; // Return execution to CPU
    }
    
    // ==========================================
    // CPU PANIC SECTOR (Vectors 0-31)
    // ==========================================
    switch (int_no) {
        case 0:
            global_framebuffer[(width * (1080/2)) + (1920/2)] = 0x0000FF00; 
            while(1) { __asm__ volatile ("hlt");}
            break;
            
        case 14: { 
            u64 faulting_address;
            __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
            print_hex_64(100, 200, faulting_address, 0x00FF0000);
            while(1) { __asm__ volatile ("hlt");}
            break;
        }
        
        default:
            // Red Screen of Death
            for (int y=0; y < height; y++) {
                for (int x=0; x < width; x++) {
                    global_framebuffer[(y * width) + x] = 0x00FF0000; 
                }
            }
            while(1) { __asm__ volatile ("hlt");}
            break;
    }
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    // Zero out the entire table to prevent undefined jumps
    uint8_t *idt_ptr = (uint8_t *)&idt;
    for (int i = 0; i < sizeof(struct idt_entry) * 256; i++) {
        idt_ptr[i] = 0;
    }

    // CPU Panics
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

    // Hardware IRQs
    idt_set_gate(32, (uint64_t)isr32);
    idt_set_gate(33, (uint64_t)isr33);
    idt_set_gate(34, (uint64_t)isr34);
    idt_set_gate(35, (uint64_t)isr35);
    idt_set_gate(36, (uint64_t)isr36);
    idt_set_gate(37, (uint64_t)isr37);
    idt_set_gate(38, (uint64_t)isr38);
    idt_set_gate(39, (uint64_t)isr39);
    idt_set_gate(40, (uint64_t)isr40);
    idt_set_gate(41, (uint64_t)isr41);
    idt_set_gate(42, (uint64_t)isr42);
    idt_set_gate(43, (uint64_t)isr43);
    idt_set_gate(44, (uint64_t)isr44);
    idt_set_gate(45, (uint64_t)isr45);
    idt_set_gate(46, (uint64_t)isr46);
    idt_set_gate(47, (uint64_t)isr47);

    // Lock the matrix into the CPU
    __asm__ volatile ("lidt %0" : : "m" (idtp));
}