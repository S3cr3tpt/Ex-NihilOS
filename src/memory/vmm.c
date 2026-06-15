#include "vmm.h"
#include "pmm.h"
#include "../global/types.h"

struct page_table* kernel_pml4;
extern u32* global_framebuffer; // Reference the screen hardware

// Bit-Masking to extract matrix coordinates
#define PML4_INDEX(addr) (((u64)(addr) >> 39) & 0x1FF)
#define PDPT_INDEX(addr) (((u64)(addr) >> 30) & 0x1FF)
#define PD_INDEX(addr)   (((u64)(addr) >> 21) & 0x1FF)
#define PT_INDEX(addr)   (((u64)(addr) >> 12) & 0x1FF)

// Strip the hardware flags to get the raw pointer
#define GET_PHYSICAL(entry) ((entry) & ~0xFFF)

void memset_zero(void* ptr, u32 size) {
    u8* p = (u8*)ptr;
    for (u32 i = 0; i < size; i++) p[i] = 0;
}

void vmm_map_page(void* virtual_address, void* physical_address, u32 flags) {
    u64 pml4_idx = PML4_INDEX(virtual_address);
    u64 pdpt_idx = PDPT_INDEX(virtual_address);
    u64 pd_idx   = PD_INDEX(virtual_address);
    u64 pt_idx   = PT_INDEX(virtual_address);

    // Level 4
    if (!(kernel_pml4->entries[pml4_idx] & PTE_PRESENT)) {
        void* new_table = pmm_alloc_block();
        memset_zero(new_table, 4096);
        kernel_pml4->entries[pml4_idx] = (u64)new_table | PTE_PRESENT | PTE_RW | PTE_USER;
    }
    struct page_table* pdpt = (struct page_table*)GET_PHYSICAL(kernel_pml4->entries[pml4_idx]);

    // Level 3
    if (!(pdpt->entries[pdpt_idx] & PTE_PRESENT)) {
        void* new_table = pmm_alloc_block();
        memset_zero(new_table, 4096);
        pdpt->entries[pdpt_idx] = (u64)new_table | PTE_PRESENT | PTE_RW | PTE_USER;
    }
    struct page_table* pd = (struct page_table*)GET_PHYSICAL(pdpt->entries[pdpt_idx]);

    // Level 2
    if (!(pd->entries[pd_idx] & PTE_PRESENT)) {
        void* new_table = pmm_alloc_block();
        memset_zero(new_table, 4096);
        pd->entries[pd_idx] = (u64)new_table | PTE_PRESENT | PTE_RW | PTE_USER;
    }
    struct page_table* pt = (struct page_table*)GET_PHYSICAL(pd->entries[pd_idx]);

    // Level 1: Lock the target physical frame
    pt->entries[pt_idx] = (u64)physical_address | flags;
}

void vmm_init() {
    // 1. Allocate the Root Node
    kernel_pml4 = (struct page_table*)pmm_alloc_block();
    memset_zero(kernel_pml4, 4096);

    // 2. Map Safe Zone 1: Kernel & Bootloader (First 16 MB = 4096 Frames)
    for (u64 i = 0; i < 4096; i++) {
        u64 addr = i * 4096;
        vmm_map_page((void*)addr, (void*)addr, PTE_PRESENT | PTE_RW);
    }

    // 3. Map Safe Zone 2: VESA Framebuffer (9 MB = 2304 Frames)
    u64 fb_base = (u64)global_framebuffer;
    for (u64 i = 0; i < 2304; i++) {
        u64 addr = fb_base + (i * 4096);
        vmm_map_page((void*)addr, (void*)addr, PTE_PRESENT | PTE_RW);
    }

    // 4. Overwrite CPU register to load the new Matrix
    __asm__ volatile ("mov %0, %%cr3" : : "r" (kernel_pml4));
}