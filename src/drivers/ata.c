#include "ata.h"
#include "../cpu/io.h"

// The hardware requires us to poll the status port until the physical drive is ready
static void ata_wait_ready() {
    // Port 0x1F7 is the Status Register. 
    // 0x80 is the BSY (Busy) bit. 0x40 is the RDY (Ready) bit.
    while (inb(0x1F7) & 0x80);        // Halt CPU while drive is busy spinning
    while (!(inb(0x1F7) & 0x40));     // Halt CPU until drive confirms ready
}

void ata_read_sector(u32 lba, u8* buffer) {
    ata_wait_ready();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F)); 
    outb(0x1F2, 1); 
    outb(0x1F3, (u8) lba);
    outb(0x1F4, (u8)(lba >> 8));
    outb(0x1F5, (u8)(lba >> 16));
    outb(0x1F7, 0x20);

    ata_wait_ready();

    // HARDWARE SHIELD: Check the Status Port for the ERR bit (0x01) or DF bit (0x20)
    u8 status = inb(0x1F7);
    if (status & 0x01 || status & 0x20) {
        // Disk fault detected. Zero the buffer to prevent memory corruption.
        for(int i = 0; i < 512; i++) buffer[i] = 0;
        return;
    }

    // Drain FIFO
    u16* ptr = (u16*) buffer;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(0x1F0);
    }
}

void ata_write_sector(u32 lba, u8* buffer) {
    ata_wait_ready();

    // 1. Send Coordinates
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F)); 
    outb(0x1F2, 1); 
    outb(0x1F3, (u8) lba);
    outb(0x1F4, (u8)(lba >> 8));
    outb(0x1F5, (u8)(lba >> 16));
    
    // 2. Issue "Write Sectors" Command (0x30)
    outb(0x1F7, 0x30);
    ata_wait_ready();

    // 3. Blast RAM buffer across the bus to the disk
    u16* ptr = (u16*) buffer;
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, ptr[i]); // Requires the inw/outw from your io.h
    }
    
    // 4. Issue Cache Flush (0xE7) to force magnetic write
    outb(0x1F7, 0xE7);
    ata_wait_ready();
}