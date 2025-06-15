#include "include/memorys.h"
#include <stdint.h>

// Define a bitmap to track allocated memory blocks
#define MEMORY_SIZE 0x1000000 // 16MB
#define BLOCK_SIZE 4096       // 4KB
#define NUM_BLOCKS (MEMORY_SIZE / BLOCK_SIZE)
#define BITMAP_SIZE (NUM_BLOCKS / 8)

static uint8_t memory_bitmap[BITMAP_SIZE];
static uint32_t memory_start;
static uint32_t memory_end;

// Function to initialize the memory manager
void init_memory_manager(uint32_t start, uint32_t end) {
    memory_start = start;
    memory_end = end;

    // Mark all blocks as free
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        memory_bitmap[i] = 0;
    }
}

// Function to allocate a block of memory
void* allocate_block() {
    // Find a free block in the bitmap
    for (uint32_t i = 0; i < NUM_BLOCKS; i++) {
        uint32_t byte_index = i / 8;
        uint32_t bit_index = i % 8;

        // Check if the block is free
        if (!(memory_bitmap[byte_index] & (1 << bit_index))) {
            // Mark the block as used
            memory_bitmap[byte_index] |= (1 << bit_index);

            // Calculate the address of the block
            uint32_t block_address = memory_start + (i * BLOCK_SIZE);

            return (void*)block_address;
        }
    }

    // No free blocks found
    return NULL;
}

// Function to free a block of memory
void free_block(void* block) {
    // Calculate the block index
    uint32_t block_address = (uint32_t)block;
    uint32_t block_index = (block_address - memory_start) / BLOCK_SIZE;

    // Check if the block index is valid
    if (block_index < NUM_BLOCKS) {
        // Mark the block as free in the bitmap
        uint32_t byte_index = block_index / 8;
        uint32_t bit_index = block_index % 8;
        memory_bitmap[byte_index] &= ~(1 << bit_index);
    }
}