#ifndef LIBALLOC_H
#define LIBALLOC_H

#include <stddef.h>

void *kmalloc(size_t size);
void kfree(void *ptr);

#endif