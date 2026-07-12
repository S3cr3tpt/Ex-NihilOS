#ifndef TUI_H
#define TUI_H
#include "../global/types.h"

void tui_init();
void tui_update_fs_panel();
void tui_update_keystate(u8 scancode);

#endif