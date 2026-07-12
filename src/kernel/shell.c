#include "shell.h"
#include "../global/types.h"
#include "../drivers/ata.h"
#include "../fs/fs.h"
#include "../video/tui.h" 

extern void draw_char(int x, int y, char c, u32 color);
extern void print_string(int x, int y, const char* str, u32 color);
extern void draw_rect(int start_x, int start_y, int width, int height, u32 color);

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

// --- COMMAND DISPATCH TABLE ---
typedef struct {
    const char* name;
    void (*handler)(const char* args);
} Command;

static void cmd_ping(const char* args) {
    print_string(cursor_x, cursor_y, "PING! Architecture Stable.", 0x0000FFFF); 
    cursor_y += 16;
}

static void cmd_help(const char* args) {
    print_string(cursor_x, cursor_y, "CORE: ping, disk, ls, cd, touch, mkdir, cat, clear", 0x0000FFFF);
    cursor_y += 16;
}

static void cmd_clear(const char* args) {
    draw_rect(302, 0, 1216, 748, 0x00000000); 
    cursor_y = 4; 
}

static void cmd_disk(const char* args) {
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
}

static void cmd_ls(const char* args) {
    fs_ls(cursor_x, &cursor_y); 
}

static void cmd_cd(const char* args) {
    if (args && *args) {
        fs_cd(args, cursor_x, &cursor_y); 
        tui_update_fs_panel();
    } else {
        print_string(cursor_x, cursor_y, "ERROR: Target directory required.", 0x00FF0000);
        cursor_y += 16;
    }
}


static void cmd_touch(const char* args) {
    if (args && *args) {
        fs_touch(args, cursor_x, &cursor_y);
        tui_update_fs_panel(); // Kinetic UI refresh
    } else {
        print_string(cursor_x, cursor_y, "ERROR: Filename required.", 0x00FF0000);
        cursor_y += 16;
    }
}

static void cmd_mkdir(const char* args) {
    if (args && *args) {
        fs_mkdir(args, cursor_x, &cursor_y);
        tui_update_fs_panel(); // Kinetic UI refresh
    } else {
        print_string(cursor_x, cursor_y, "ERROR: Directory name required.", 0x00FF0000);
        cursor_y += 16;
    }
}

static void cmd_cat(const char* args) {
    if (args && *args) {
        fs_cat(args, cursor_x, &cursor_y);
    } else {
        print_string(cursor_x, cursor_y, "ERROR: Filename required.", 0x00FF0000);
        cursor_y += 16;
    }
}

static Command command_table[] = {
    {"ping", cmd_ping},
    {"help", cmd_help},
    {"clear", cmd_clear},
    {"disk", cmd_disk},
    {"ls", cmd_ls},
    {"cd", cmd_cd},
    {"touch", cmd_touch}, 
    {"mkdir", cmd_mkdir}, 
    {"cat", cmd_cat},     
    {0, 0}
};
// --- REPL EXECUTION LOGIC ---
void shell_init() {
    tui_init(); // Boot graphical interface
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

    if (cmd_index > 0) {
        char* cmd_name = command_buffer;
        char* args = 0;
        for (int i = 0; i < cmd_index; i++) {
            if (command_buffer[i] == ' ') {
                command_buffer[i] = '\0';
                args = &command_buffer[i + 1];
                break;
            }
        }

        int executed = 0;
        for (int i = 0; command_table[i].name != 0; i++) {
            if (kstrcmp(cmd_name, command_table[i].name) == 0) {
                command_table[i].handler(args);
                executed = 1;
                break;
            }
        }
        if (!executed) {
            print_string(cursor_x, cursor_y, "ERROR: Command Unrecognized.", 0x00FF0000); 
            cursor_y += 16;
        }
    }

    for(int i = 0; i < 256; i++) command_buffer[i] = 0;
    cmd_index = 0;
    cursor_x = 320;
    if (cursor_y >= 730) { draw_rect(302, 0, 1216, 748, 0x00000000); cursor_y = 20; }
    print_string(cursor_x, cursor_y, "EX-NIHILOS > ", 0x0000FF00);
    cursor_x += 8 * 13;
}

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