#ifndef ATA_H
#define ATA_H

#include "../global/types.h"

// Pulls a raw 512-byte sector from the disk into the provided RAM buffer
void ata_read_sector(u32 lba, u8* buffer);
void ata_write_sector(u32 lba, u8* buffer);
#endif