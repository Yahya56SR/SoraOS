#include "include/liballoc.h"
#include "include/memorys.h" // Include your memory management header

void *kmalloc(size_t size) {
    return memorys_kmalloc(size); // Use the kmalloc function from memorys.h
}

void kfree(void *ptr) {
    memorys_kfree(ptr); // Use the kfree function from memorys.h
}