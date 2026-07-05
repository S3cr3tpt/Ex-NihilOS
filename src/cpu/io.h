#ifndef IO_H
#define IO_H

#include "../global/types.h"

// Send a byte to a specific hardware port
static inline void outb(u16 port, u8 val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Read a byte from a specific hardware port
static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Force the CPU to wait one clock cycle for the legacy PIC hardware to catch up
static inline void io_wait(void) {
    outb(0x80, 0); // Port 0x80 is used for 'checkpoints' and safely burns a cycle
}

#endif