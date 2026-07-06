#include "font.h"
#include "../global/types.h"

extern u32* global_framebuffer;
#define SCREEN_WIDTH 1920

// 1. New Core Logic: Draw a specific ASCII character
void draw_char(int x, int y, char c, u32 color) {
    // Hardware Safety: Ensure char is within valid ASCII range
    if (c < 0 || c > 127) return; 

    for (int row = 0; row < 8; row++) {
        unsigned char row_data = font8x8[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                u64 offset = ((y + row) * SCREEN_WIDTH) + (x + col);
                global_framebuffer[offset] = color;
            }
        }
    }
}

// 2. New Core Logic: Print an entire string of text
void print_string(int x, int y, const char* str, u32 color) {
    int current_x = x;
    while (*str) {
        draw_char(current_x, y, *str, color);
        current_x += 8; // Advance cursor 8 pixels right for next character
        str++;
    }
}

// --- Legacy Overrides: Rewired to use new standard ASCII array ---

void print_hex_64(int start_x, int start_y, u64 value, u32 color) {
    draw_char(start_x, start_y, '0', color);
    draw_char(start_x + 8, start_y, 'x', color);
    int current_x = start_x + 16; 

    for (int i = 60; i >= 0; i -= 4) {
        u8 nibble = (value >> i) & 0xF;
        // Ascii Math: Convert 0-9 to '0'-'9', and 10-15 to 'A'-'F'
        char hex_char = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
        draw_char(current_x, start_y, hex_char, color);
        current_x += 8;
    }
}

int print_dec_32(int start_x, int start_y, u32 value, u32 color) {
    if (value == 0) {
        draw_char(start_x, start_y, '0', color);
        return start_x + 8;
    }

    int buffer[10]; 
    int i = 0;
    
    while (value > 0) {
        buffer[i++] = value % 10;
        value /= 10;
    }

    int current_x = start_x;
    for (int j = i - 1; j >= 0; j--) {
        draw_char(current_x, start_y, '0' + buffer[j], color); 
        current_x += 8;
    }
    
    return current_x; 
}

void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color) {
    u32 mb_value = (u32)(total_bytes >> 20);
    int current_x = print_dec_32(start_x, start_y, mb_value, color);
    
    draw_char(current_x, start_y, ' ', color);
    current_x += 8;
    draw_char(current_x, start_y, 'M', color);
    current_x += 8;
    draw_char(current_x, start_y, 'B', color);
}

// Blasts a solid block of color to physical memory (Used for erasing and UI borders)
void draw_rect(int start_x, int start_y, int width, int height, u32 color) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Hardware Bounds Check
            if (start_x + x < 1920 && start_y + y < 1080) {
                global_framebuffer[(start_y + y) * 1920 + (start_x + x)] = color;
            }
        }
    }
}