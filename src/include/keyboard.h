#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Remove namespace Kernel

inline uint8_t keyboard_inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#endif