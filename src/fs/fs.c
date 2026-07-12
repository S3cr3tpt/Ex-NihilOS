#include "fs.h"
#include "../drivers/ata.h"
#include "../utils/sort.h"
extern void print_string(int x, int y, const char* str, u32 color);

static u32 current_dir_lba = 100;
static char current_path[128] = "root";

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

    // Execute Bidirectional Algorithmic Sort
    cocktail_sort_filenodes(nodes, 16);

    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0) {
            u32 color = (nodes[i].type == 2) ? 0x0088FFFF : 0x00FFFFFF; // Blue for Dir, White for File
            print_string(term_x, *term_y, nodes[i].name, color);
            *term_y += 16;
        }
    }
}


static void fs_path_append(const char* folder) {
    int len = 0;
    while (current_path[len] != '\0' && len < 120) len++;
    
    current_path[len++] = '/';
    while (*folder != '\0' && len < 126) {
        current_path[len++] = *folder++;
    }
    current_path[len] = '\0';
}

static void fs_path_pop() {
    int len = 0;
    while (current_path[len] != '\0') len++;
    
    // Step backward until we hit the parent separator
    while (len > 0 && current_path[len] != '/') {
        len--;
    }
    
    // Null-terminate at the slash to sever the child directory
    if (len > 0) {
        current_path[len] = '\0';
    } else {
        // Fallback: If at absolute base, guarantee structural root binding
        fs_strcpy(current_path, "root");
    }
}

void fs_cd(const char* target, int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    for (int i = 0; i < 16; i++) {
        if (nodes[i].type == 2 && fs_strcmp(nodes[i].name, target) == 0) {
            current_dir_lba = nodes[i].lba;
            
            if (fs_strcmp(target, "..") == 0) {
                fs_path_pop(); // Step back one branch in the UI tree
            } else {
                fs_path_append(target); // Push new branch onto UI tree
            }
            return;
        }
    }
    
    print_string(term_x, *term_y, "ERROR: Directory not found.", 0x00FF0000);
    *term_y += 16;
}

void fs_render_panel(int start_x, int start_y) {
    u32 cyan  = 0x0000FFFF;
    u32 white = 0x00FFFFFF;
    u32 blue  = 0x0088FFFF;
    u32 grey  = 0x00555555;
    
    print_string(start_x, start_y, "[ FILE SYSTEM ]", cyan);
    print_string(start_x, start_y + 40, "> ", white);
    print_string(start_x + 16, start_y + 40, current_path, cyan);
    
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;
    
    // 1. Algorithmic Intercept: Cocktail Shaker Sort (Directories -> Files -> A-Z)
    cocktail_sort_filenodes(nodes, 16);

    int draw_y = start_y + 80;
    
    // 2. Single Linear Sweep over mathematically ordered nodes
    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0) {
            print_string(start_x, draw_y, "  |- ", grey);
            u32 color = (nodes[i].type == 2) ? blue : white;
            print_string(start_x + 40, draw_y, nodes[i].name, color);
            draw_y += 20;
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

static u32 next_free_lba = 104; // LBA 100-103 are reserved for initial format

static u32 fs_alloc_sector() {
    u8 buffer[512];
    while (next_free_lba < 4096) { // 2MB Platter Limit
        ata_read_sector(next_free_lba, buffer);
        u32* check = (u32*)buffer;
        
        // If the first 4 bytes are 0x00, sector is unallocated physical space
        if (*check == 0 && buffer[4] == 0) {
            // Zero out sector completely to guarantee clean execution state
            for(int i = 0; i < 512; i++) buffer[i] = 0;
            ata_write_sector(next_free_lba, buffer);
            return next_free_lba++;
        }
        next_free_lba++;
    }
    return 0; // Disk Full Fault
}

void fs_touch(const char* filename, int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    // 1. Check for duplicates and locate first empty slot
    int empty_slot = -1;
    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0 && fs_strcmp(nodes[i].name, filename) == 0) {
            print_string(term_x, *term_y, "ERROR: File already exists.", 0x00FF0000);
            *term_y += 16;
            return;
        }
        if (nodes[i].type == 0 && empty_slot == -1) {
            empty_slot = i;
        }
    }

    if (empty_slot == -1) {
        print_string(term_x, *term_y, "ERROR: Directory sector full (Max 16 nodes).", 0x00FF0000);
        *term_y += 16;
        return;
    }

    // 2. Allocate physical SSD block and populate node
    u32 new_lba = fs_alloc_sector();
    if (new_lba == 0) {
        print_string(term_x, *term_y, "ERROR: Storage platter full.", 0x00FF0000);
        *term_y += 16;
        return;
    }

    fs_strcpy(nodes[empty_slot].name, filename);
    nodes[empty_slot].type = 1; // Type 1 = File
    nodes[empty_slot].lba = new_lba;
    nodes[empty_slot].size = 0;

    // 3. Burn routing table back to metal
    ata_write_sector(current_dir_lba, buffer);
}

void fs_mkdir(const char* dirname, int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    int empty_slot = -1;
    for (int i = 0; i < 16; i++) {
        if (nodes[i].type != 0 && fs_strcmp(nodes[i].name, dirname) == 0) {
            print_string(term_x, *term_y, "ERROR: Directory already exists.", 0x00FF0000);
            *term_y += 16;
            return;
        }
        if (nodes[i].type == 0 && empty_slot == -1) {
            empty_slot = i;
        }
    }

    if (empty_slot == -1) {
        print_string(term_x, *term_y, "ERROR: Directory sector full.", 0x00FF0000);
        *term_y += 16;
        return;
    }

    u32 new_lba = fs_alloc_sector();
    if (new_lba == 0) {
        print_string(term_x, *term_y, "ERROR: Storage platter full.", 0x00FF0000);
        *term_y += 16;
        return;
    }

    // 1. Populate parent directory node
    fs_strcpy(nodes[empty_slot].name, dirname);
    nodes[empty_slot].type = 2; // Type 2 = Directory
    nodes[empty_slot].lba = new_lba;
    nodes[empty_slot].size = 0;
    ata_write_sector(current_dir_lba, buffer);

    // 2. Format new child sector with structural parent link (..)
    u8 child_buffer[512];
    for(int i = 0; i < 512; i++) child_buffer[i] = 0;
    FileNode* child_nodes = (FileNode*)child_buffer;

    fs_strcpy(child_nodes[0].name, "..");
    child_nodes[0].type = 2;
    child_nodes[0].lba = current_dir_lba; // Route back to parent coordinate
    
    ata_write_sector(new_lba, child_buffer);
}


void fs_cat(const char* filename, int term_x, int* term_y) {
    u8 buffer[512];
    ata_read_sector(current_dir_lba, buffer);
    FileNode* nodes = (FileNode*)buffer;

    for (int i = 0; i < 16; i++) {
        if (nodes[i].type == 1 && fs_strcmp(nodes[i].name, filename) == 0) {
            u32 target_lba = nodes[i].lba;
            
            u8 payload[512];
            ata_read_sector(target_lba, payload);
            
            // Print payload string to terminal until null terminator or sector end
            char line_buf[65]; // Buffer to print cleanly across terminal width
            int line_idx = 0;
            
            for (int b = 0; b < 512; b++) {
                if (payload[b] == '\0') break; // End of text data
                
                if (payload[b] == '\n' || line_idx >= 64) {
                    line_buf[line_idx] = '\0';
                    print_string(term_x, *term_y, line_buf, 0x00FFFFFF);
                    *term_y += 16;
                    line_idx = 0;
                    if (payload[b] == '\n') continue;
                }
                line_buf[line_idx++] = payload[b];
            }
            if (line_idx > 0) {
                line_buf[line_idx] = '\0';
                print_string(term_x, *term_y, line_buf, 0x00FFFFFF);
                *term_y += 16;
            }
            return;
        }
    }
    
    print_string(term_x, *term_y, "ERROR: File not found or is a directory.", 0x00FF0000);
    *term_y += 16;
}