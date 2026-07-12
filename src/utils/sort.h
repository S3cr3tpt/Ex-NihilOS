#ifndef SORT_H
#define SORT_H
#include "../global/types.h"
#include "../fs/fs.h"

// Sorts FileNode arrays: Directories (Type 2) first, then Alphabetical A->Z
void cocktail_sort_filenodes(FileNode* nodes, int count);

#endif