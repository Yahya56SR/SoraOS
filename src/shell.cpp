#include "include/shell.h"
#include "include/process/process_manager.h"
#include "include/kernel_util.h"
#include <stddef.h>

namespace Kernel {

// Initialize static members
uint16_t* const Shell::vga_buffer = (uint16_t*)0xB8000;
volatile size_t Shell::terminal_row = 0;
volatile size_t Shell::terminal_col = 0;
char Shell::cmd_buffer[CMD_BUFFER_SIZE];
size_t Shell::cmd_pos = 0;

// Shell version and build info
static const char* const SHELL_VERSION = "1.0.0";
static const char* const BUILD_DATE = "2025-06-11 21:33:15";  // Updated timestamp
static const char* const USERNAME = "Yahya56SR";

void Shell::print_char(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            // Scroll screen
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
                }
            }
            // Clear last line
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(VGA_COLOR_LIGHT_GREY << 8);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }
    
    vga_buffer[terminal_row * VGA_WIDTH + terminal_col] = (uint16_t)c | (uint16_t)(VGA_COLOR_LIGHT_GREY << 8);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
            // Scroll screen
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + 1) * VGA_WIDTH + x];
                }
            }
            // Clear last line
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(VGA_COLOR_LIGHT_GREY << 8);
            }
        }
    }
}

void Shell::handle_input(char c) {
    if (c == '\n') {
        print_char('\n');
        cmd_buffer[cmd_pos] = '\0';
        process_command(cmd_buffer);
        cmd_pos = 0;
    }
    else if (c == '\b' && cmd_pos > 0) {
        backspace();
        cmd_pos--;
    }
    else if (c >= ' ' && c <= '~' && cmd_pos < CMD_BUFFER_SIZE - 1) {
        print_char(c);
        cmd_buffer[cmd_pos++] = c;
    }
}

void Shell::backspace() {
    if (terminal_col > 2) {  // Don't erase the prompt
        terminal_col--;
        vga_buffer[terminal_row * VGA_WIDTH + terminal_col] = (uint16_t)' ' | (uint16_t)(VGA_COLOR_LIGHT_GREY << 8);
    }
}

void Shell::print(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

void Shell::clear_screen() {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(VGA_COLOR_LIGHT_GREY << 8);
        }
    }
    terminal_row = 0;
    terminal_col = 0;
}

static bool str_equals(const char* a, const char* b) {
    while (*a && *b) {
        if (*a++ != *b++) return false;
    }
    return *a == *b;
}

static bool str_starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) return false;
    }
    return true;
}

void Shell::initialize() {
    clear_screen();
    cmd_pos = 0;
    
    // Print welcome message
    print("SoraOS Shell v");
    print(SHELL_VERSION);
    print(" - Built: ");
    print(BUILD_DATE);
    print("\n");
    print("User: ");
    print(USERNAME);
    print("\n\n");
    print("Type 'help' for available commands\n\n");
    print("$ ");
}

void Shell::process_command(const char* cmd) {
    // Skip leading spaces
    while (*cmd == ' ') cmd++;
    
    // Empty command
    if (*cmd == '\0') {
        print("$ ");
        return;
    }
    
    // Process commands
    if (str_equals(cmd, "clear")) {
        clear_screen();
    }
    else if (str_equals(cmd, "help")) {
        cmd_help();
    }
    else if (str_equals(cmd, "ls")) {
        cmd_ls();
    }
    else if (str_starts_with(cmd, "run ")) {
        cmd_run(cmd + 4);  // Skip "run "
    }
    else if (str_equals(cmd, "version")) {
        print("SoraOS Shell v");
        print(SHELL_VERSION);
        print("\nBuild date: ");
        print(BUILD_DATE);
        print("\nUser: ");
        print(USERNAME);
        print("\n");
    }
    else if (str_equals(cmd, "exit")) {
        print("System shutdown not implemented\n");
    }
    else {
        print("Unknown command: ");
        print(cmd);
        print("\nType 'help' for available commands\n");
    }
    
    print("$ ");
}

void Shell::cmd_help() {
    print("Available commands:\n");
    print("  clear    - Clear the screen\n");
    print("  help     - Show this help message\n");
    print("  ls       - List available executables\n");
    print("  run      - Run a Sora executable (run <filename>)\n");
    print("  version  - Show shell version information\n");
    print("  exit     - Shutdown the system (not implemented)\n");
}

void Shell::cmd_ls() {
    print("Available Sora executables:\n");
    print("  counter.sora    - Simple counter application\n");
    print("  calculator.sora - Basic calculator\n");
    print("  editor.sora     - Text editor\n");
}

void Shell::cmd_run(const char* args) {
    // Skip leading spaces
    while (*args == ' ') args++;
    
    if (*args == '\0') {
        print("Usage: run <filename>\n");
        return;
    }
    
    print("Running Sora executable: ");
    print(args);
    print("\n");
    
    // Create process for the Sora executable
    Process* process = ProcessManager::create_process(args);
    if (!process) {
        print("Error: Failed to create process\n");
        return;
    }
    
    // Load and run the executable
    if (!ProcessManager::start_process(process)) {
        print("Error: Failed to start process\n");
        ProcessManager::destroy_process(process);
        return;
    }
    
    print("Process started successfully\n");
}

} // namespace Kernel