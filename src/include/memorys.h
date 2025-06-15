#pragma once
#include <stdint.h>
#include <stddef.h>

// Memory pool for allocations (defined in memorys.c)
#define MEMORY_POOL_SIZE (1024 * 1024) // 1MB
extern char memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(16)));
extern size_t pool_index;

// Function to allocate memory from the pool
void *kmalloc(size_t size);

// Function to free memory (no-op for bump allocator)
void kfree(void *ptr);

// Function to set memory
void kmemset(void *dest, int val, size_t len);

// Function to copy memory
void kmemcpy(void *dest, const void *src, size_t len);

// Function to read a byte from an I/O port
uint8_t inb(uint16_t port);

// Function to write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}