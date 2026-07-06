#include "keyboard.h"

// Base Map (Lowercase & Standard Numbers)
const char kbd_US[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',       
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',            
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,             
  '*',   0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,         
    0,   0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,         
    0,   0,   0,   0,   0,   0                                                  
};

// Shifted Map (Uppercase & Symbols)
const char kbd_US_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',   
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',       
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',            
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0,             
  '*',   0, ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,         
    0,   0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,         
    0,   0,   0,   0,   0,   0                                                  
};

// The State Machine Tracker (0 = off, 1 = on)
static int shift_active = 0;

char keyboard_process_scancode(u8 scancode) {
    // 1. Intercept Shift Make (Press)
    // 0x2A = Left Shift, 0x36 = Right Shift
    if (scancode == 0x2A || scancode == 0x36) {
        shift_active = 1;
        return 0; // Hardware state changed; no ASCII to print
    }

    // 2. Intercept Shift Break (Release)
    // Break code = Make code + 0x80 (0xAA and 0xB6)
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_active = 0;
        return 0; // Hardware state changed; no ASCII to print
    }

    // 3. The General Signal Filter: Ignore all other Break Codes
    if (scancode & 0x80) {
        return 0; 
    }

    // 4. Hardware Bounds Check
    if (scancode >= 128) {
        return 0; 
    }

    // 5. The Cipher Routing
    // Divert the memory read to the correct array based on the physical Shift state
    if (shift_active) {
        return kbd_US_shift[scancode];
    } else {
        return kbd_US[scancode];
    }
}