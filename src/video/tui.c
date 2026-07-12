#include "tui.h"
#include "../global/types.h"
#include "../fs/fs.h"

extern void draw_char(int x, int y, char c, u32 color);
extern void print_string(int x, int y, const char* str, u32 color);
extern void draw_rect(int start_x, int start_y, int width, int height, u32 color);
extern void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color);
extern u64 total_memory_size;
extern u64 used_blocks;

// --- KINETIC KEYBOARD MATRIX ---
static int kb_x[128] = {0};
static int kb_y[128] = {0};
static int kb_w[128] = {0};
static const char* kb_label[128] = {0};

static void tui_draw_key(u8 scancode, int active) {
    if (scancode >= 128 || kb_x[scancode] == 0) return; 
    u32 cyan = 0x0000FFFF, white = 0x00FFFFFF, black = 0x00000000;
    u32 text_col = active ? black : white;
    u32 bg_col   = active ? white : black;
    int h = 50; 

    draw_rect(kb_x[scancode], kb_y[scancode], kb_w[scancode], h, cyan);
    draw_rect(kb_x[scancode] + 2, kb_y[scancode] + 2, kb_w[scancode] - 4, h - 4, bg_col);

    int len = 0;
    const char* str = kb_label[scancode];
    while(str[len]) len++;
    int text_x = kb_x[scancode] + (kb_w[scancode] / 2) - ((len * 8) / 2);
    int text_y = kb_y[scancode] + (h / 2) - 4; 

    for(int i = 0; i < len; i++) {
        draw_char(text_x + (i * 8), text_y, str[i], text_col);
    }
}

static void map_key(u8 sc, int x, int y, int w, const char* label) {
    kb_x[sc] = x; kb_y[sc] = y; kb_w[sc] = w; kb_label[sc] = label;
    tui_draw_key(sc, 0); 
}

static void init_keyboard_map() {
    print_string(430, 770, "[ VIRTUAL SENSOR MAP ]", 0x0000FFFF);

    int basex = 430;
    int std_w = 85;           
    int step = std_w + 10;    

    // Row 1 (Numbers)
    int sx = basex; int sy = 790;
    map_key(0x29, sx, sy, std_w, "`"); sx += step;
    map_key(0x02, sx, sy, std_w, "1"); sx += step;
    map_key(0x03, sx, sy, std_w, "2"); sx += step;
    map_key(0x04, sx, sy, std_w, "3"); sx += step;
    map_key(0x05, sx, sy, std_w, "4"); sx += step;
    map_key(0x06, sx, sy, std_w, "5"); sx += step;
    map_key(0x07, sx, sy, std_w, "6"); sx += step;
    map_key(0x08, sx, sy, std_w, "7"); sx += step;
    map_key(0x09, sx, sy, std_w, "8"); sx += step;
    map_key(0x0A, sx, sy, std_w, "9"); sx += step;
    map_key(0x0B, sx, sy, std_w, "0"); sx += step;
    map_key(0x0C, sx, sy, std_w, "-"); sx += step;
    map_key(0x0D, sx, sy, std_w, "="); sx += step;
    map_key(0x0E, sx, sy, 215, "BACKSPACE"); 

    // Row 2 (QWERTY)
    sx = basex; sy += 60;
    map_key(0x0F, sx, sy, 135, "TAB"); sx += 145;
    map_key(0x10, sx, sy, std_w, "Q"); sx += step;
    map_key(0x11, sx, sy, std_w, "W"); sx += step;
    map_key(0x12, sx, sy, std_w, "E"); sx += step;
    map_key(0x13, sx, sy, std_w, "R"); sx += step;
    map_key(0x14, sx, sy, std_w, "T"); sx += step;
    map_key(0x15, sx, sy, std_w, "Y"); sx += step;
    map_key(0x16, sx, sy, std_w, "U"); sx += step;
    map_key(0x17, sx, sy, std_w, "I"); sx += step;
    map_key(0x18, sx, sy, std_w, "O"); sx += step;
    map_key(0x19, sx, sy, std_w, "P"); sx += step;
    map_key(0x1A, sx, sy, std_w, "["); sx += step;
    map_key(0x1B, sx, sy, std_w, "]"); sx += step;
    map_key(0x2B, sx, sy, 165, "\\");

    // Row 3 (ASDF)
    sx = basex; sy += 60;
    map_key(0x3A, sx, sy, 175, "CAPS"); sx += 185;
    map_key(0x1E, sx, sy, std_w, "A"); sx += step;
    map_key(0x1F, sx, sy, std_w, "S"); sx += step;
    map_key(0x20, sx, sy, std_w, "D"); sx += step;
    map_key(0x21, sx, sy, std_w, "F"); sx += step;
    map_key(0x22, sx, sy, std_w, "G"); sx += step;
    map_key(0x23, sx, sy, std_w, "H"); sx += step;
    map_key(0x24, sx, sy, std_w, "J"); sx += step;
    map_key(0x25, sx, sy, std_w, "K"); sx += step;
    map_key(0x26, sx, sy, std_w, "L"); sx += step;
    map_key(0x27, sx, sy, std_w, ";"); sx += step;
    map_key(0x28, sx, sy, std_w, "'"); sx += step;
    map_key(0x1C, sx, sy, 220, "ENTER");

    // Row 4 (ZXCV)
    sx = basex; sy += 60;
    map_key(0x2A, sx, sy, 225, "SHIFT"); sx += 235;
    map_key(0x2C, sx, sy, std_w, "Z"); sx += step;
    map_key(0x2D, sx, sy, std_w, "X"); sx += step;
    map_key(0x2E, sx, sy, std_w, "C"); sx += step;
    map_key(0x2F, sx, sy, std_w, "V"); sx += step;
    map_key(0x30, sx, sy, std_w, "B"); sx += step;
    map_key(0x31, sx, sy, std_w, "N"); sx += step;
    map_key(0x32, sx, sy, std_w, "M"); sx += step;
    map_key(0x33, sx, sy, std_w, ","); sx += step;
    map_key(0x34, sx, sy, std_w, "."); sx += step;
    map_key(0x35, sx, sy, std_w, "/"); sx += step;
    map_key(0x36, sx, sy, 265, "SHIFT");

    // Row 5 (Space)
    sx = basex; sy += 60;
    map_key(0x1D, sx, sy, 250, "CTRL"); sx += 260;
    map_key(0x38, sx, sy, 250, "ALT"); sx += 260;
    map_key(0x39, sx, sy, 930, "SPACE");}

static void draw_telemetry() {
    u32 cyan  = 0x0000FFFF;
    u32 white = 0x00FFFFFF;
    u32 grey  = 0x00555555;
    u32 green = 0x0000FF00;
    
    print_string(20, 20, "[ SYSTEM STATE ]", cyan);
    print_string(20, 60, "CPU: x86_64 Core", white);
    print_string(20, 80, "MODE: Long Mode (Ring 0)", grey);
    print_string(20, 100, "ARCH: Ex-NihilOS Bare Metal", grey);

    // --- MEMORY MATRIX ---
    print_string(20, 160, "[ MEMORY MATRIX ]", cyan);
    
    // 1. Total Installed RAM (Calculated via E820 BIOS Map)
    print_string(20, 200, "TOTAL:", white);
    print_memory_string(80, 200, total_memory_size, white);
    
    // 2. Dynamic Used RAM (Physical Frames * 4096 Bytes)
    u64 used_memory_bytes = used_blocks * 4096;
    print_string(20, 220, "USED :", grey);
    print_memory_string(80, 220, used_memory_bytes, green);

    // 3. Dynamic Available RAM (Total - Used)
    u64 avail_bytes = (total_memory_size > used_memory_bytes) ? (total_memory_size - used_memory_bytes) : 0;
    print_string(20, 240, "AVAIL:", white);
    print_memory_string(80, 240, avail_bytes, cyan);

    // --- STORAGE MATRIX ---
    print_string(20, 300, "[ STORAGE BUS ]", cyan);
    print_string(20, 340, "DRIVE: ATA PIO Mode", white);
    print_string(20, 360, "SIZE : 2048 KB Platter", grey);
    print_string(20, 380, "SECT : LBA 0 -> LBA 4096", grey);
}

static void draw_network() {
    u32 cyan = 0x0000FFFF, white = 0x00FFFFFF, grey = 0x00555555;
    print_string(20, 770, "[ NETWORK STACK ]", cyan);
    print_string(20, 810, "INTERFACE: eth0", white);
    print_string(20, 830, "STATUS: OFFLINE", grey);
}

void tui_update_fs_panel() {
    draw_rect(1522, 0, 398, 750, 0x00000000); 
    fs_render_panel(1540, 20); 
}

void tui_init() {
    draw_rect(0, 0, 1920, 1080, 0x00000000);
    u32 cyan = 0x0000FFFF;
    draw_rect(0, 750, 1920, 2, cyan);
    draw_rect(300, 0, 2, 750, cyan);
    draw_rect(1520, 0, 2, 750, cyan);
    draw_rect(400, 750, 2, 330, cyan);

    draw_telemetry();
    tui_update_fs_panel();
    draw_network();
    init_keyboard_map();
}

void tui_update_keystate(u8 scancode) {
    int break_code = (scancode & 0x80) ? 1 : 0;
    u8 base_sc = scancode & 0x7F; 
    tui_draw_key(base_sc, !break_code);
}