#pragma once
#include "kernel_util.h"

inline void operator delete(void* ptr) {
    kfree(ptr);
}

// Placement new
inline void* operator new(unsigned int, void* ptr) {
    return ptr;
}