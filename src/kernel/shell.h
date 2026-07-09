#ifndef SHELL_H
#define SHELL_H

#include "../global/types.h"

void shell_init();
void shell_process_char(char c);

// New: Direct hardware visualizer link
void shell_update_keystate(u8 scancode);

#endif