#include "shell.h"
#include "../global/types.h"

extern void draw_char(int x, int y, char c, u32 color);
extern void print_string(int x, int y, const char* str, u32 color);
extern void draw_rect(int start_x, int start_y, int width, int height, u32 color); // New Link

static char command_buffer[256];
static int cmd_index = 0;

// Confine cursor to the Center Panel (Terminal)
static int cursor_x = 420;
static int cursor_y = 60;
static u32 term_color = 0x00FFFFFF;

static int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// ---------------------------------------------------------
// TUI RENDERER (The Day 0 Matrix)
// ---------------------------------------------------------
static void tui_init() {
    // 1. Wipe the entire screen to absolute black
    draw_rect(0, 0, 1920, 1080, 0x00000000);
    
    u32 border = 0x00555555; // Tactical Grey
    u32 text = 0x0000FF00;   // Terminal Green
    
    // 2. Draw Tiling Borders
    draw_rect(0, 800, 1920, 2, border);    // Horizontal divider (Top workspaces vs Keyboard)
    draw_rect(400, 0, 2, 800, border);     // Vertical divider 1 (System State | Terminal)
    draw_rect(1520, 0, 2, 800, border);    // Vertical divider 2 (Terminal | Files)
    
    // 3. Render Sector Labels
    print_string(20, 20, "[ CPU / RAM / BATT ]", text);
    print_string(20, 40, "(System State)", 0x00888888);

    print_string(420, 20, "[ TERMINAL ]", text);
    print_string(420, 40, "(God Mode)", 0x00888888);

    print_string(1540, 20, "[ FILES ]", text);
    print_string(1540, 40, "(Context)", 0x00888888);

    print_string(20, 820, "[ VIRTUAL KEYBOARD ]", text);
    print_string(20, 840, "(Visualizer / Macro Map)", 0x00888888);
}

// ---------------------------------------------------------
// REPL LOGIC
// ---------------------------------------------------------
void shell_init() {
    tui_init();
    print_string(cursor_x, cursor_y, "EX-NIHILOS > ", 0x0000FF00);
    cursor_x += 8 * 13; 
}

static void shell_execute() {
    cursor_y += 16; 
    cursor_x = 420; // Snap back to the left margin of the Terminal panel

    // Boundary Check: If terminal hits the bottom of the panel, wipe it and reset to top
    if (cursor_y >= 760) {
        draw_rect(402, 60, 1116, 738, 0x00000000); // Erase only the Terminal workspace
        cursor_y = 60;
    }

    if (cmd_index == 0) { } 
    else if (kstrcmp(command_buffer, "ping") == 0) {
        print_string(cursor_x, cursor_y, "PONG! OS Architecture Stable.", 0x0000FFFF); 
        cursor_y += 16;
    } 
    else if (kstrcmp(command_buffer, "help") == 0) {
        print_string(cursor_x, cursor_y, "CORE: ping, help, clear", 0x0000FFFF); 
        cursor_y += 16;
    }
    else if (kstrcmp(command_buffer, "clear") == 0) {
        draw_rect(402, 60, 1116, 738, 0x00000000); 
        cursor_y = 44; // Will be pushed to 60 below
    }
    else {
        print_string(cursor_x, cursor_y, "ERROR: Command Unrecognized.", 0x00FF0000); 
        cursor_y += 16;
    }

    // Memory Reset
    for(int i = 0; i < 256; i++) command_buffer[i] = 0;
    cmd_index = 0;
    
    // Spawn next prompt
    cursor_x = 420;
    if (cursor_y >= 760) {
        draw_rect(402, 60, 1116, 738, 0x00000000);
        cursor_y = 60;
    }
    print_string(cursor_x, cursor_y, "EX-NIHILOS > ", 0x0000FF00);
    cursor_x += 8 * 13;
}

void shell_process_char(char c) {
    if (c == '\n') {
        shell_execute();
        return;
    }
    
    if (c == '\b') {
        if (cmd_index > 0) {
            cmd_index--;
            command_buffer[cmd_index] = 0; 
            cursor_x -= 8; 
            // KINETIC ERASE: Blast 8x8 black square over the character
            draw_rect(cursor_x, cursor_y, 8, 8, 0x00000000); 
        }
        return;
    }

    // Terminal Bounds Check: Prevent typing past the right border
    if (cmd_index < 255 && c >= 32 && c <= 126 && cursor_x < 1500) {
        command_buffer[cmd_index++] = c;
        draw_char(cursor_x, cursor_y, c, term_color);
        cursor_x += 8;
    }
}