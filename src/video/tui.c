#include "tui.h"
#include "../global/types.h"
#include "../fs/fs.h"

extern void draw_char(int x, int y, char c, u32 color);
extern void print_string(int x, int y, const char* str, u32 color);
extern void draw_rect(int start_x, int start_y, int width, int height, u32 color);
extern void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color);
extern u64 total_memory_size;

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
    int basex = 430, std_w = 85, step = std_w + 10;    
    int sx = basex, sy = 790;
    
    // Paste exact map_key calls from old shell.c here...
}

static void draw_telemetry() {
    u32 cyan = 0x0000FFFF, white = 0x00FFFFFF, grey = 0x00555555;
    print_string(20, 20, "[ SYSTEM STATE ]", cyan);
    print_string(20, 80, "CPU: x86_64 Core", white);
    print_string(20, 260, "RAM:", white);
    print_memory_string(60, 260, total_memory_size, white);
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