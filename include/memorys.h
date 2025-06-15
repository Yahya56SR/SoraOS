#ifndef MEMORYS_H
#define MEMORYS_H

#include <stddef.h>

void *operator new(size_t size) noexcept
{
    return nullptr; // Out of memory
}

void *operator new[](size_t size) noexcept
{
    return nullptr; // Out of memory
}

void operator delete(void *ptr) noexcept
{
    // Do nothing since we don't have memory management yet
}

void operator delete[](void *ptr) noexcept
{
    // Do nothing since we don't have memory management yet
}

#endif // MEMORYS_H
