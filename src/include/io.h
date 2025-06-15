#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h> // For size_t

// Assuming these are defined in vga.h or a similar header
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Assuming these are defined in keyboard.h or a similar header
#define ENTER 0x1C
#define SHIFT_PRESSED_LEFT 0x2A
#define SHIFT_RELEASED_LEFT 0xAA
#define SHIFT_PRESSED_RIGHT 0x36
#define SHIFT_RELEASED_RIGHT 0xB6
#define BACKSPACE 0x0E
#define KEY_LIMIT 0x3A

// C doesn't have namespaces, so we need to flatten the function names
// If you have naming conflicts, rename these functions

uint8_t keyboard_inb(uint16_t port);

// Declare global variables
extern size_t cursor_x;
extern size_t cursor_y;
extern uint16_t* vga_buffer;

// Declare functions
void scroll_screen(uint16_t* buffer);

char scancode_to_ascii(uint8_t scancode, int shift_pressed);
void print_char(char c, int inplace, int color);
void print_string(const char *str, int color);
void print_int(int x, int color);
uint8_t scankey();
void input(char* buffer, size_t buffer_size, int color);

// Declare scancode arrays
extern uint8_t scancode_ascii_normal[];
extern uint8_t scancode_ascii_shifted[];

#endif