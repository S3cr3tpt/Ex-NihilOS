#include "shell.h"
#include "../global/types.h"
#include "../drivers/ata.h"
#include "../fs/fs.h"

extern void draw_char(int x, int y, char c, u32 color);
extern void print_string(int x, int y, const char* str, u32 color);
extern void draw_rect(int start_x, int start_y, int width, int height, u32 color);
extern void print_memory_string(int start_x, int start_y, u64 total_bytes, u32 color);

extern u64 total_memory_size;

static char command_buffer[256];
static int cmd_index = 0;

static int cursor_x = 320;
static int cursor_y = 60;
static u32 term_color = 0x00FFFFFF;

static int kstrncmp(const char* s1, const char* s2, int n) {
    while (n && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// ---------------------------------------------------------
// KINETIC KEYBOARD MATRIX (O(1) COORDINATE TRACKING)
// ---------------------------------------------------------
static int kb_x[128] = {0};
static int kb_y[128] = {0};
static int kb_w[128] = {0};
static const char* kb_label[128] = {0};

static void tui_draw_key(u8 scancode, int active) {
    if (scancode >= 128 || kb_x[scancode] == 0) return; 

    u32 cyan = 0x0000FFFF;
    u32 white = 0x00FFFFFF;
    u32 black = 0x00000000;

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

    int cx = text_x;
    for(int i = 0; i < len; i++) {
        draw_char(cx, text_y, str[i], text_col);
        cx += 8;
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
    map_key(0x39, sx, sy, 930, "SPACE");
}

// ---------------------------------------------------------
// TUI RENDERER: PANELS & SCENERY
// ---------------------------------------------------------
static void draw_telemetry() {
    u32 cyan = 0x0000FFFF, white = 0x00FFFFFF, grey = 0x00555555;
    print_string(20, 20, "[ SYSTEM STATE ]", cyan);
    print_string(20, 80, "CPU: x86_64 Core", white);
    print_string(20, 100, "LOAD: 12% (MSR REQ)", grey);
    print_string(20, 120, "TEMP: 45C (ACPI REQ)", grey); 
    print_string(20, 180, "GPU: VESA FB", white);
    print_string(20, 200, "TEMP: N/A", grey);
    print_string(20, 260, "RAM:", white);
    print_memory_string(60, 260, total_memory_size, white);
    print_string(20, 280, "USE: 14 MB (STATIC)", grey);
    print_string(20, 340, "BATT: ACPI OFFLINE", grey);
}

static void draw_network() {
    u32 cyan = 0x0000FFFF, white = 0x00FFFFFF, grey = 0x00555555;
    print_string(20, 770, "[ NETWORK STACK ]", cyan);
    print_string(20, 810, "INTERFACE: eth0", white);
    print_string(20, 830, "STATUS: OFFLINE (NO PHY)", grey);
    print_string(20, 870, "MAC: 00:00:00:00:00:00", white);
    print_string(20, 890, "IPv4: UNBOUND", white);
    print_string(20, 930, "TX: 0 BYTES", grey);
    print_string(20, 950, "RX: 0 BYTES", grey);
    print_string(20, 1010, "FIREWALL: DEFAULT DENY", white);
}

static void tui_update_fs_panel() {
    // Blast exact remaining geometry to black to prevent screen artifacts
    draw_rect(1522, 0, 398, 750, 0x00000000); 
    fs_render_panel(1540, 20); 
}

static void tui_init() {
    draw_rect(0, 0, 1920, 1080, 0x00000000);
    u32 cyan = 0x0000FFFF;

    draw_rect(0, 750, 1920, 2, cyan);    // Horizontal (Bottom Panels)
    draw_rect(300, 0, 2, 750, cyan);     // Vertical 1 (Telemetry)
    draw_rect(1520, 0, 2, 750, cyan);    // Vertical 2 (Filesystem)
    draw_rect(400, 750, 2, 330, cyan);   // Vertical 3 (Network vs Keyboard)

    draw_telemetry();
    tui_update_fs_panel();
    draw_network();
    init_keyboard_map();
}

// ---------------------------------------------------------
// REPL EXECUTION LOGIC
// ---------------------------------------------------------
void shell_init() {
    tui_init();
    print_string(cursor_x, cursor_y, "EX-NIHILOS > ", 0x0000FF00);
    cursor_x += 8 * 13; 
}

static void shell_execute() {
    cursor_y += 16; 
    cursor_x = 320; 

    if (cursor_y >= 730) {
        draw_rect(302, 0, 1216, 748, 0x00000000); 
        cursor_y = 20;
    }

    if (cmd_index == 0) { } 
    else if (kstrcmp(command_buffer, "ping") == 0) {
        print_string(cursor_x, cursor_y, "PING! Architecture Stable.", 0x0000FFFF); 
        cursor_y += 16;
    }
    else if (kstrcmp(command_buffer, "help") == 0) {
        print_string(cursor_x, cursor_y, "CORE: ping, disk, ls, cd, clear", 0x0000FFFF); 
        cursor_y += 16;
    } 
    else if (kstrcmp(command_buffer, "clear") == 0) {
        draw_rect(302, 0, 1216, 748, 0x00000000); 
        cursor_y = 4; 
    }
    else if (kstrcmp(command_buffer, "disk") == 0) {
        print_string(cursor_x, cursor_y, "Initiating ATA PIO Hardware Scan...", 0x0000FFFF);
        cursor_y += 16;
        
        u8 disk_buffer[512]; 
        ata_read_sector(0, disk_buffer); 
        
        u16 signature = disk_buffer[510] | (disk_buffer[511] << 8);
        
        if (signature == 0xAA55) {
            print_string(cursor_x, cursor_y, "[OK] Sector 0 Read Verified (0xAA55).", 0x0000FF00); 
        } else {
            print_string(cursor_x, cursor_y, "[FAULT] Hardware Read Failed.", 0x00FF0000); 
        }
        cursor_y += 16;
    }else if (kstrcmp(command_buffer, "ls") == 0) {
        fs_ls(cursor_x, &cursor_y); 
    }
    else if (kstrncmp(command_buffer, "cd ", 3) == 0) { // [PATCH]: Syntax corrected to kstrncmp
        fs_cd(command_buffer + 3, cursor_x, &cursor_y); 
        tui_update_fs_panel(); 
    }
    else {
        print_string(cursor_x, cursor_y, "ERROR: Command Unrecognized.", 0x00FF0000); 
        cursor_y += 16;
    }

    for(int i = 0; i < 256; i++) command_buffer[i] = 0;
    cmd_index = 0;
    
    cursor_x = 320;
    if (cursor_y >= 730) { draw_rect(302, 0, 1216, 748, 0x00000000); cursor_y = 20; }
    print_string(cursor_x, cursor_y, "EX-NIHILOS > ", 0x0000FF00);
    cursor_x += 8 * 13;
}

// 1. Raw Hardware Intercept (Fixes the highlighting bug)
void shell_update_keystate(u8 scancode) {
    int break_code = (scancode & 0x80) ? 1 : 0;
    u8 base_sc = scancode & 0x7F; 
    tui_draw_key(base_sc, !break_code);
}

// 2. Logic Intercept (Only runs on valid ASCII characters)
void shell_process_char(char c) {
    if (c == '\n') { shell_execute(); return; }
    
    if (c == '\b') {
        if (cmd_index > 0) {
            cmd_index--;
            command_buffer[cmd_index] = 0; 
            cursor_x -= 8; 
            draw_rect(cursor_x, cursor_y, 8, 8, 0x00000000); 
        }
        return;
    }

    if (cmd_index < 255 && c >= 32 && c <= 126 && cursor_x < 1500) {
        command_buffer[cmd_index++] = c;
        draw_char(cursor_x, cursor_y, c, term_color);
        cursor_x += 8;
    }
}