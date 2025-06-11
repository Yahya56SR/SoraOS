#include <stddef.h>
#include <stdint.h>

extern "C" {

// Simple bump allocator for kernel heap
static uint8_t kernel_heap[1024 * 1024]; // 1MB kernel heap
static size_t heap_ptr = 0;

void* kmalloc(size_t size) {
    if (heap_ptr + size > sizeof(kernel_heap)) {
        return nullptr;
    }
    void* ptr = &kernel_heap[heap_ptr];
    heap_ptr += (size + 7) & ~7; // 8-byte alignment
    return ptr;
}

void kfree(void* ptr) {
    // Simple implementation - no actual freeing
    (void)ptr;
}

void kmemset(void* dest, int val, size_t len) {
    uint8_t* d = (uint8_t*)dest;
    while (len--) {
        *d++ = (uint8_t)val;
    }
}

void kmemcpy(void* dest, const void* src, size_t len) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while (len--) {
        *d++ = *s++;
    }
}

} // extern "C"