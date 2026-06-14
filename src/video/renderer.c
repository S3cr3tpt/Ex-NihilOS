#include "font.h"
#include "../global/types.h"

extern u32* global_framebuffer;
#define SCREEN_WIDTH 1920

void draw_hex_char(int x, int y, int font_index, u32 color) {
    for (int row = 0; row < 8; row++) {
        unsigned char row_data = minimal_font[font_index][row];
        for (int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                u64 offset = ((y + row) * SCREEN_WIDTH) + (x + col);
                global_framebuffer[offset] = color;
            }
        }
    }
}

void print_hex_64(int start_x, int start_y, u64 value, u32 color) {
    draw_hex_char(start_x, start_y, 16, color); // Print 'x'
    int current_x = start_x + 10; 

    for (int i = 60; i >= 0; i -= 4) {
        u8 nibble = (value >> i) & 0xF;
        draw_hex_char(current_x, start_y, nibble, color);
        current_x += 10;
    }
}
// ... [Existing draw_hex_char and print_hex_64 code stays here] ...

// The Base-10 Integer Rendering Logic
int print_dec_32(int start_x, int start_y, u32 value, u32 color) {
    if (value == 0) {
        draw_hex_char(start_x, start_y, 0, color);
        return start_x + 10;
    }

    int buffer[10]; // 32-bit max is 4,294,967,295 (10 digits max)
    int i = 0;
    
    // Extract digits
    while (value > 0) {
        buffer[i++] = value % 10;
        value /= 10;
    }

    int current_x = start_x;
    
    // Print digits in reverse order (left to right)
    for (int j = i - 1; j >= 0; j--) {
        draw_hex_char(current_x, start_y, buffer[j], color); // Indices 0-9 match exactly
        current_x += 10;
    }
    
    return current_x; // Return the new cursor X position
}

// Function to print the memory string: e.g., "128 MB"
void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color) {
    // Bitshift by 20 to convert Bytes to Megabytes
    u32 mb_value = (u32)(total_bytes >> 20);
    
    // Print the numeric value and get the new X coordinate
    int current_x = print_dec_32(start_x, start_y, mb_value, color);
    
    // Print the Space, 'M', and 'B'
    draw_hex_char(current_x, start_y, 19, color); // Space (Index 19)
    current_x += 10;
    draw_hex_char(current_x, start_y, 17, color); // 'M' (Index 17)
    current_x += 10;
    draw_hex_char(current_x, start_y, 11, color); // 'B' (Index 11)
}