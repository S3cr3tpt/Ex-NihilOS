#ifndef FS_H
#define FS_H
#include "../global/types.h"

// The 32-Byte Index Node (16 fit exactly in a 512-byte sector)
typedef struct {
    char name[23];
    u8 type;     // 0 = Empty, 1 = File, 2 = Directory
    u32 lba;     // The physical sector where this data/dir starts
    u32 size;    // Size in bytes
} __attribute__((packed)) FileNode;

void fs_format();
void fs_ls(int term_x, int* term_y);
void fs_cd(const char* target, int term_x, int* term_y);
void fs_render_panel(int start_x, int start_y);
void fs_init();

void fs_touch(const char* filename, int term_x, int* term_y);
void fs_mkdir(const char* dirname, int term_x, int* term_y);
void fs_cat(const char* filename, int term_x, int* term_y);
#endif