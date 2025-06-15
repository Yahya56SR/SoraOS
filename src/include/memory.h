#pragma once
#include "kernel_util.h"

// Memory management functions
void *kmalloc(size_t size);
void kfree(void *ptr);
