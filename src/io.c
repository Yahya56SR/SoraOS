#include "include/liballoc.h"
#include <stdbool.h> // Include stdbool.h
#include <stdint.h>
#include <stddef.h> // For size_t
#include <string.h> // Include string.h
#include "include/vectors.h"
#include "include/consts.h"

// Define the I/O ports for the keyboard
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Function to read a byte from an I/O port
uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Function to write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Function to read a byte from the keyboard
uint8_t keyboard_inb(uint16_t port) {
    return inb(port);
}

// Function to print a character to the screen
void print_char(char character, uint8_t color) {
    static uint16_t* video_memory = (uint16_t*)0xB8000;
    static uint8_t cursor_x = 0;
    static uint8_t cursor_y = 0;

    // Handle backspace
    if (character == 0x08 && cursor_x) {
        cursor_x--;
    }
    // Handle tab
    else if (character == 0x09) {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }
    // Handle carriage return
    else if (character == '\r') {
        cursor_x = 0;
    }
    // Handle newline
    else if (character == '\n') {
        cursor_x = 0;
        cursor_y++;
    }
    // Handle printable characters
    else if (character >= ' ') {
        uint16_t attribute = color << 8;
        uint16_t* screen_char = video_memory + (cursor_y * 80 + cursor_x);
        *screen_char = character | attribute;
        cursor_x++;
    }

    // Check if cursor needs to wrap
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll the screen if necessary
    if (cursor_y >= 25) {
        // Copy each line to the line above it
        for (size_t y = 0; y < 24; y++) {
            for (size_t x = 0; x < 80; x++) {
                video_memory[y * 80 + x] = video_memory[(y + 1) * 80 + x];
            }
        }

        // Clear the last line
        for (size_t x = 0; x < 80; x++) {
            video_memory[24 * 80 + x] = 0;
        }

        cursor_y = 24;
    }
}

// Function to print a string to the screen
void print_string(const char* str, uint8_t color) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        print_char(str[i], color);
    }
}

// Function to clear the screen
void clear_screen(uint8_t color) {
    uint16_t attribute = color << 8;
    uint16_t* video_memory = (uint16_t*)0xB8000;
    for (size_t y = 0; y < 25; y++) {
        for (size_t x = 0; x < 80; x++) {
            video_memory[y * 80 + x] = ' ' | attribute;
        }
    }
}

// Function to get input from the keyboard
void input(char* buffer, size_t max_size, uint8_t color) {
    size_t i = 0;
    uint8_t keycode;

    clear_screen(VGA_COLOR_BLACK);
    print_string("Enter text: ", color);

    while (true) {
        // Wait for a key to be pressed
        do {
            keycode = keyboard_inb(KEYBOARD_DATA_PORT);
        } while (keycode == 0);

        // Check if the key is a printable character
        if (keycode >= 0x02 && keycode <= 0x39) {
            char character = keycode + 29; // Convert keycode to ASCII
            if (i < max_size - 1) {
                buffer[i++] = character;
                print_char(character, color);
            }
        }
        // Check if the key is backspace
        else if (keycode == 0x0E && i > 0) {
            i--;
            print_char(0x08, color); // Backspace character
        }
        // Check if the key is enter
        else if (keycode == 0x1C) {
            buffer[i] = '\0';
            print_char('\n', color);
            break;
        }
    }
}

// Function to concatenate multiple strings into a single buffer
size_t concat_strings(char* dest, size_t dest_size, const char* src[], size_t num_src) {
    size_t total_length = 0;
    for (size_t i = 0; i < num_src; ++i) {
        total_length += strlen(src[i]);
    }

    if (total_length >= dest_size) {
        // Handle the error: string is too long to fit in the destination buffer
        return 0;
    }

    dest[0] = '\0'; // Initialize the destination buffer to an empty string
    for (size_t i = 0; i < num_src; ++i) {
        strcat(dest, src[i]);
    }

    return total_length;
}