#include "fs.h"
#include "../drivers/ata.h"

extern void print_string(int x, int y, const char* str, u32 color);

static u32 current_dir_lba = 100;
static char current_path[64] = "root"; // Telemetry tracker

static int fs_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static void fs_strcpy(char* dest, const char* src) {
    while (*src) { *dest++ = *src++; }
    *dest = 0;
}

void fs_format() {
    u8 buffer[512];
    for(int i=0; i<512; i++) buffer[i] = 0; 
    FileNode* nodes = (FileNode*)buffer;

    fs_strcpy(nodes[0].name, "sys");
    nodes[0].type = 2; 
    nodes[0].lba = 101; 
    
    fs_strcpy(nodes[1].name, "DEVLOG.md");
    nodes[1].type = 1; 
    nodes[1].lba = 102;
    
    ata_write_sector(100, buffer); 

    for(int i=0; i<512; i++) buffer[i] = 0; 
    
    fs_strcpy(nodes[0].name, "..");
    nodes[0].type = 2; 
    nodes[0].lba = 100; 
    
    fs_strcpy(nodes[1].name, "kernel.bin");
    nodes[1].type = 1;
    nodes[1].lba = 103;
    
    ata_write_sector(101, buffer); 
}

void fs_ls(int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0) {
            u32 color = (nodes[i].type == 2) ? 0x0088FFFF : 0x00FFFFFF; 
            print_string(term_x, *term_y, nodes[i].name, color);
            *term_y += 16;
        }
    }
}

void fs_cd(const char* target, int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    for (int i = 0; i < 16; i++) {
        if (nodes[i].type == 2 && fs_strcmp(nodes[i].name, target) == 0) {
            current_dir_lba = nodes[i].lba; 
            
            // Kinetic Path Tracking
            if (fs_strcmp(target, "..") == 0) {
                fs_strcpy(current_path, "root"); // Hard-reset visualizer if moving back
            } else {
                fs_strcpy(current_path, target);
            }
            return;
        }
    }
    
    print_string(term_x, *term_y, "ERROR: Directory not found.", 0x00FF0000);
    *term_y += 16;
}

// THE UI RENDER MATRIX
void fs_render_panel(int start_x, int start_y) {
    u32 cyan = 0x0000FFFF;
    u32 white = 0x00FFFFFF;
    u32 blue = 0x0088FFFF; // Bright blue for Directory visibility
    u32 grey = 0x00555555;
    
    print_string(start_x, start_y, "[ FILE SYSTEM ]", cyan);
    
    print_string(start_x, start_y + 40, "> ", white);
    print_string(start_x + 16, start_y + 40, current_path, cyan);
    
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;
    
    int draw_y = start_y + 80;
    
    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0) {
            print_string(start_x, draw_y, "  |- ", grey);
            u32 color = (nodes[i].type == 2) ? blue : white;
            print_string(start_x + 40, draw_y, nodes[i].name, color);
            draw_y += 20; // 20px step for visual clarity
        }
    }
}

// Evaluates the root sector and autonomous formats if raw
void fs_init() {
    u8 buffer[512];
    ata_read_sector(100, buffer);
    FileNode* nodes = (FileNode*)buffer;

    // A raw sector will have type 0 and a null-byte name.
    if (nodes[0].type == 0 && nodes[0].name[0] == '\0') {
        fs_format();
    }
}