#include "include/memorys.h"

// Memory pool for allocations
#define MEMORY_POOL_SIZE (1024 * 1024) // 1MB
char memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
size_t pool_index = 0;

// Function to allocate memory from the pool
void *kmalloc(size_t size) {
    if (pool_index + size > MEMORY_POOL_SIZE) {
        return NULL; // Out of memory
    }
    void *ptr = &memory_pool[pool_index];
    pool_index += size;
    return ptr;
}

// Function to free memory (no-op for bump allocator)
void kfree(void *ptr) {
    // Do nothing for bump allocator
    (void)ptr;
}

// Function to set memory
void kmemset(void *dest, int val, size_t len) {
    unsigned char *ptr = (unsigned char *)dest;
    while (len--) {
        *ptr++ = (unsigned char)val;
    }
}

// Function to copy memory
void kmemcpy(void *dest, const void *src, size_t len) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    while (len--) {
        *d++ = *s++;
    }
}

// Function to read a byte from an I/O port
uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Function to write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}