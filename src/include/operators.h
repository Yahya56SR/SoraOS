#pragma once
#include "kernel_util.h"

inline void operator delete(void* ptr) {
    kfree(ptr);
}

inline void operator delete(void* ptr, unsigned int size) {
    (void)size; // Suppress unused parameter warning
    if (ptr) {
        kfree(ptr); // Or your custom memory deallocation
    }
}