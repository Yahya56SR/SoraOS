#include "include/keyboard.h"
#include "include/interrupts.h"
#include "include/shell.h"
#include "include/memorys.h"  // Use your existing memory header

namespace Kernel {
namespace Keyboard {

// Keyboard buffer
static volatile char buffer[BUFFER_SIZE];
static volatile size_t write_pos = 0;
static volatile size_t read_pos = 0;

// Keyboard state
static volatile bool shift_pressed = false;
static volatile bool caps_lock = false;

// Scancode to ASCII lookup tables
static const char ascii_lowercase[] = {
    0,   0,   '1', '2', '3',  '4', '5', '6', '7',  '8', '9',  '0',  '-',  '=',  '\b',
    '\t', 'q', 'w', 'e', 'r',  't', 'y', 'u', 'i',  'o', 'p',  '[',  ']',  '\n',
    0,   'a', 's', 'd', 'f',  'g', 'h', 'j', 'k',  'l', ';',  '\'', '`',
    0,   '\\','z', 'x', 'c',  'v', 'b', 'n', 'm',  ',', '.',  '/',  0,
    '*',  0,   ' '
};

static const char ascii_uppercase[] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' '
};

// Add character to buffer
static void buffer_put(char c) {
    size_t next = (write_pos + 1) % BUFFER_SIZE;
    if (next != read_pos) {
        buffer[write_pos] = c;
        write_pos = next;
    }
}

void initialize() {
    // Reset buffer positions
    write_pos = 0;
    read_pos = 0;
    
    // Wait for keyboard controller to be ready
    while ((inb(STATUS_PORT) & 2) != 0) {
        asm volatile("pause");
    }
    
    // Enable keyboard
    outb(COMMAND_PORT, 0xAE);
    
    // Wait again
    while ((inb(STATUS_PORT) & 2) != 0) {
        asm volatile("pause");
    }
    
    // Enable scanning
    outb(DATA_PORT, 0xF4);
    
    // Install keyboard handler
    InterruptManager::set_handler(0x21, keyboard_interrupt_handler);
}

// Define the handler with extern "C" to avoid name mangling
extern "C" void keyboard_handler() {
    uint8_t scancode = inb(DATA_PORT);
    
    // Handle key release
    if (scancode & 0x80) {
        scancode &= 0x7F;
        
        // Handle shift release
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = false;
        }
        
        return;
    }
    
    // Handle key press
    switch (scancode) {
        case 0x2A: // Left shift
        case 0x36: // Right shift
            shift_pressed = true;
            break;
            
        case 0x3A: // Caps lock
            caps_lock = !caps_lock;
            break;
            
        default:
            if (scancode < sizeof(ascii_lowercase)) {
                char c;
                bool uppercase = (shift_pressed != caps_lock);
                
                if (uppercase) {
                    c = ascii_uppercase[scancode];
                } else {
                    c = ascii_lowercase[scancode];
                }
                
                if (c != 0) {
                    buffer_put(c);
                }
            }
            break;
    }
    
    // Send EOI to PIC
    outb(0x20, 0x20);
}

bool is_key_available() {
    return read_pos != write_pos;
}

char get_char() {
    while (!is_key_available()) {
        asm volatile("pause");
    }
    
    char c = buffer[read_pos];
    read_pos = (read_pos + 1) % BUFFER_SIZE;
    return c;
}

}} // namespace Kernel::Keyboard