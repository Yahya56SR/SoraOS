#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Kernel {
namespace Keyboard {

// Keyboard ports
static const uint16_t DATA_PORT = 0x60;
static const uint16_t STATUS_PORT = 0x64;
static const uint16_t COMMAND_PORT = 0x64;

// Buffer size
static const size_t BUFFER_SIZE = 256;

// Initialize keyboard
void initialize();

// Get character from keyboard buffer
char get_char();

// Check if key is available
bool is_key_available();

// External functions for keyboard interrupt handling
extern "C" void keyboard_interrupt_handler();
extern "C" void keyboard_handler(); // Make it extern "C" to prevent mangling

// Port I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

}} // namespace Kernel::Keyboard