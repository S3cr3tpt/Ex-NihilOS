#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

// Offset1 = Master PIC target vector (We will use 0x20 / 32)
// Offset2 = Slave PIC target vector (We will use 0x28 / 40)
void pic_remap(u32 offset1, u32 offset2) {
    u8 a1, a2;

    // 1. Save the current hardware interrupt masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // 2. ICW1: Wake up and start initialization sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); io_wait();

    // 3. ICW2: Set the new interrupt vector offsets (Clearing the 0-31 panic zone)
    outb(PIC1_DATA, offset1); io_wait();
    outb(PIC2_DATA, offset2); io_wait();

    // 4. ICW3: Establish the cascade wiring (Tell Master it has a Slave at IRQ2)
    outb(PIC1_DATA, 4); io_wait(); 
    outb(PIC2_DATA, 2); io_wait(); 

    // 5. ICW4: Set standard 8086 operation mode
    outb(PIC1_DATA, ICW4_8086); io_wait();
    outb(PIC2_DATA, ICW4_8086); io_wait();

    // 6. Restore the saved hardware interrupt masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

// Disable a specific hardware interrupt
void pic_set_mask(u8 irq_line) {
    u16 port;
    u8 value;

    if(irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) | (1 << irq_line);
    outb(port, value);
}

// Enable a specific hardware interrupt (Unmask)
void pic_clear_mask(u8 irq_line) {
    u16 port;
    u8 value;

    if(irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}