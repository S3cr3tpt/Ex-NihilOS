#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../global/types.h"

// Translates a raw hardware scancode into an ASCII character
char keyboard_process_scancode(u8 scancode);

#endif