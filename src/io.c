#include "include/io.h"
#include "include/vectors.h" // For strlen
#include "include/liballoc.h"
#include <stdbool.h>

size_t cursor_x = 0;
size_t cursor_y = 0;
uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Define the arrays (assuming they are in keyboard.cpp or similar)
uint8_t scancode_ascii_normal[] = { /* ... your scancode to ASCII mapping ... */ };
uint8_t scancode_ascii_shifted[] = { /* ... your shifted scancode to ASCII mapping ... */ };

void scroll_screen(uint16_t* buffer) {
    // ... your scroll screen implementation ...
}

char scancode_to_ascii(uint8_t scancode, int shift_pressed) {
    return shift_pressed ? scancode_ascii_shifted[scancode] : scancode_ascii_normal[scancode];
}

void print_char(char c, int inplace, int color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        uint16_t position = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[position] = (color << 8) | c;
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll_screen(vga_buffer);
    }
}

void print_string(const char *str, int color) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
        print_char(str[i], 0, color); //inplace is always 0
    }
}

void print_int(int x, int color) {
    char str[12]; // Enough to hold -2147483648 + null terminator
    int i = 0;
    int is_negative = 0;

    if (x < 0) {
        is_negative = 1;
        x = -x;
    }

    if (x == 0) {
        str[i++] = '0';
    } else {
        while (x > 0) {
            str[i++] = (x % 10) + '0';
            x /= 10;
        }
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }

    print_string(str, color);
}

uint8_t scankey() {
    while (!(keyboard_inb(0x64) & 0x1));
    return keyboard_inb(0x60);
}

void input(char* buffer, size_t buffer_size, int color) {
    size_t visual_cursor_x = 0;
    size_t visual_cursor_y = cursor_y;
    size_t current_index = 0;
    int shift_pressed = 0;

    while (true) {
        uint8_t scancode = scankey();

        switch (scancode) {
            case ENTER:
                buffer[current_index] = '\0';
                print_char('\n', 0, color);
                return;
            case SHIFT_PRESSED_LEFT:
            case SHIFT_PRESSED_RIGHT:
                shift_pressed = 1;
                break;
            case SHIFT_RELEASED_LEFT:
            case SHIFT_RELEASED_RIGHT:
                shift_pressed = 0;
                break;
            case BACKSPACE:
                if (current_index > 0) {
                    current_index--;
                    if (cursor_x > 0) {
                        cursor_x--;
                    } else {
                        cursor_y--;
                        cursor_x = VGA_WIDTH - 1;
                    }
                    uint16_t position = cursor_y * VGA_WIDTH + cursor_x;
                    vga_buffer[position] = (color << 8) | ' ';
                }
                break;
            default:
                if (current_index < buffer_size - 1) {
                    if (scancode < KEY_LIMIT) {
                        char c = scancode_to_ascii(scancode, shift_pressed);
                        buffer[current_index++] = c;
                        print_char(c, 0, color);
                    }
                }
                break;
        }
    }
}