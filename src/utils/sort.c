#include "sort.h"
#include "../fs/fs.h"

static int sort_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void cocktail_sort_filenodes(FileNode* nodes, int count) {
    int swapped = 1;
    int start = 0;
    int end = count - 1;

    while (swapped) {
        swapped = 0;
        
        // --- PASS 1: LEFT TO RIGHT (Push heavy files / Z to the right) ---
        for (int i = start; i < end; ++i) {
            int swap_needed = 0;
            
            // Empty nodes (Type 0) always get pushed to the absolute end
            if (nodes[i].type == 0 && nodes[i + 1].type != 0) {
                swap_needed = 1;
            }
            // Primary Key: Directories (Type 2) must precede Files (Type 1)
            else if (nodes[i].type == 1 && nodes[i + 1].type == 2) {
                swap_needed = 1;
            }
            // Secondary Key: If types are identical, sort alphabetically A -> Z
            else if (nodes[i].type != 0 && nodes[i].type == nodes[i + 1].type) {
                if (sort_strcmp(nodes[i].name, nodes[i + 1].name) > 0) {
                    swap_needed = 1;
                }
            }

            if (swap_needed) {
                FileNode temp = nodes[i];
                nodes[i] = nodes[i + 1];
                nodes[i + 1] = temp;
                swapped = 1;
            }
        }

        if (!swapped) break;
        swapped = 0;
        --end;

        // --- PASS 2: RIGHT TO LEFT (Pull light directories / A to the left) ---
        for (int i = end - 1; i >= start; --i) {
            int swap_needed = 0;

            if (nodes[i].type == 0 && nodes[i + 1].type != 0) {
                swap_needed = 1;
            }
            else if (nodes[i].type == 1 && nodes[i + 1].type == 2) {
                swap_needed = 1;
            }
            else if (nodes[i].type != 0 && nodes[i].type == nodes[i + 1].type) {
                if (sort_strcmp(nodes[i].name, nodes[i + 1].name) > 0) {
                    swap_needed = 1;
                }
            }

            if (swap_needed) {
                FileNode temp = nodes[i];
                nodes[i] = nodes[i + 1];
                nodes[i + 1] = temp;
                swapped = 1;
            }
        }
        ++start;
    }
}