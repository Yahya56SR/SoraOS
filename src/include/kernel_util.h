#pragma once
#include <stddef.h>

// Simple heap implementation
static char heap[1024 * 1024]; // 1MB heap
static size_t heap_pos = 0;

void *kmalloc(size_t size)
{
    // Align size to 4 bytes
    size = (size + 3) & ~3;

    if (heap_pos + size > sizeof(heap))
    {
        return nullptr;
    }

    void *ptr = &heap[heap_pos];
    heap_pos += size;
    return ptr;
}

void kfree(void *ptr)
{
    // Simple implementation - no actual freeing
    (void)ptr;
}