#pragma once
#include <stddef.h>
#include <stdint.h>

namespace Kernel {

class Shell {
public:
    static void initialize();
    static void process_command(const char* cmd);
    
    // Terminal output functions
    static void print_char(char c);
    static void print(const char* str);
    static void backspace();
    static void clear_screen();
    
    // Input handling
    static void handle_input(char c);
    
private:
    static void cmd_run(const char* args);
    static void cmd_help();
    static void cmd_ls();
    
    // Terminal state
    static uint16_t* const vga_buffer;
    static volatile size_t terminal_row;
    static volatile size_t terminal_col;
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;
    static const uint8_t VGA_COLOR_LIGHT_GREY = 7;
    
    // Command buffer
    static const size_t CMD_BUFFER_SIZE = 256;
    static char cmd_buffer[CMD_BUFFER_SIZE];
    static size_t cmd_pos;
};

} // namespace Kernel