#include "heap.h"
#include "vmm.h"
#include "pmm.h"

#define HEAP_START 0x2000000 

heap_block_t* heap_head = (heap_block_t*)1; // Placeholder to avoid NULL dereference before initialization

void heap_init() {
    heap_head = 0; // Reset the heap head to NULL before initialization
    // 1. Request raw physical frame
    void* initial_block = pmm_alloc_block();
    
    // System Halt: PMM is exhausted
    if (initial_block == 0) return; 

    // 2. Map physical frame to virtual heap address using explicit macros
    vmm_map_page((void*)HEAP_START, initial_block, PTE_PRESENT | PTE_RW); 
    
    // 3. CPU CACHE OVERRIDE (CRITICAL)
    // Force the processor to drop the outdated memory cache for this exact address.
    // Without this instruction, the next line triggers the Red Screen.
    __asm__ volatile("invlpg (%0)" : : "r" (HEAP_START) : "memory");

    // 4. Construct the master block
    heap_head = (heap_block_t*)HEAP_START;
    heap_head->size = 4096 - sizeof(heap_block_t); 
    heap_head->is_free = 1;
    heap_head->next = 0;
}

void* kmalloc(u32 size) {
    if (size == 0) return 0;

    heap_block_t* current = heap_head;
    
    // Sweep the list for an available sector
    while (current != 0) {
        if (current->is_free && current->size >= size) {
            
            // Execute Block Split if remaining space justifies a new header
            if (current->size > size + sizeof(heap_block_t) + 4) {
                heap_block_t* split_block = (heap_block_t*)((u8*)current + sizeof(heap_block_t) + size);
                split_block->size = current->size - size - sizeof(heap_block_t);
                split_block->is_free = 1;
                split_block->next = current->next;
                
                current->size = size;
                current->next = split_block;
            }
            
            current->is_free = 0;
            // Return pointer directly to the payload, bypassing the header
            return (void*)((u8*)current + sizeof(heap_block_t));
        }
        current = current->next;
    }
    
    return 0; // Heap Exhaustion
}

void kfree(void* ptr) {
    if (ptr == 0) return;
    
    // Reverse pointer arithmetic to intercept the header
    heap_block_t* header = (heap_block_t*)((u8*)ptr - sizeof(heap_block_t));
    header->is_free = 1;
    
    // Coalesce: Merge with adjacent block to prevent fragmentation
    if (header->next != 0 && header->next->is_free) {
        header->size += sizeof(heap_block_t) + header->next->size;
        header->next = header->next->next;
    }
}