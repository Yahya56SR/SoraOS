#include "include/operators.h"
#include "include/memory.h"

void *operator new[](size_t size)
{
    return kmalloc(size);
}

void operator delete[](void *p)
{
    kfree(p);
}

void operator delete[](void *p, size_t)
{
    kfree(p);
}