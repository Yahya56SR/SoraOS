#include "include/paging.h"
#include "include/tasking.h"

// Simulated I/O functions and types for demo purposes
#include <stdint.h>

// Dummy VGA color values for print_string
#define VGA_COLOR_LIGHT_CYAN 0x0B
#define VGA_COLOR_GREEN 0x02
#define VGA_COLOR_BLUE 0x01

// Dummy print_string implementation (replace with real one)
void print_string(const char* str, int color = VGA_COLOR_LIGHT_CYAN) {
    (void)color;
    while (*str) {
        // Output character to screen here
        ++str;
    }
}

// Dummy print_int implementation
void print_int(int value) {
    char buf[16];
    int i = 0;
    if (value == 0) {
        print_string("0");
        return;
    }
    bool neg = value < 0;
    if (neg) value = -value;
    while (value && i < 15) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }
    if (neg) buf[i++] = '-';
    buf[i] = 0;
    // reverse and print
    for (int j = i - 1; j >= 0; --j)
        print_string(&buf[j]);
}

// Dummy clear screen function
void cls() {}

// Simulated input function (stub, replace with keyboard input)
void input(char* buf, int maxlen = 32) {
    // For a real kernel, you'd read from keyboard buffer into buf
    buf[0] = 0;
}

// Minimal vector<char> type for demonstration
struct vector_char {
    char data[256];
    int sz;
    vector_char() : sz(0) {}
    void push_back(char c) { if (sz < 255) data[sz++] = c; }
    void clear() { sz = 0; }
    int size() const { return sz; }
    char& operator[](int i) { return data[i]; }
    const char& operator[](int i) const { return data[i]; }
};

// Converts a vector_char to int (simple atoi)
int to_int(const vector_char& v) {
    int result = 0;
    bool negative = false;
    int i = 0;
    if (v.sz > 0 && v[0] == '-') { negative = true; i = 1; }
    for (; i < v.sz; ++i) {
        if (v[i] < '0' || v[i] > '9') break;
        result = result * 10 + (v[i] - '0');
    }
    return negative ? -result : result;
}

// Converts int to vector_char
vector_char int_to_vector(int value) {
    char buf[16];
    int idx = 0;
    bool negative = false;
    if (value < 0) { negative = true; value = -value; }
    do { buf[idx++] = '0' + (value % 10); value /= 10; } while (value);
    if (negative) buf[idx++] = '-';
    vector_char v;
    for (int i = idx - 1; i >= 0; --i) v.push_back(buf[i]);
    return v;
}

// Helper to print vector_char
const char* vector_to_cstr(const vector_char& v) {
    static char buf[256];
    int n = (v.sz < 255) ? v.sz : 255;
    for (int i = 0; i < n; ++i) buf[i] = v[i];
    buf[n] = 0;
    return buf;
}

// CMOS I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Read a byte from the CMOS register
uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

// Convert BCD to binary
uint8_t bcd_to_bin(uint8_t val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

// Get current year from RTC (BIOS)
int get_current_year_from_rtc() {
    uint8_t year = cmos_read(0x09);     // Year (last two digits)
    uint8_t century = cmos_read(0x32);  // CMOS century register (if present)
    uint8_t status_b = cmos_read(0x0B);

    // If not in binary mode, convert from BCD
    if (!(status_b & 0x04)) {
        year = bcd_to_bin(year);
        century = bcd_to_bin(century);
    }

    if (century) {
        return century * 100 + year;
    } else {
        return 2000 + year;
    }
}

// Returns the user's age, or asks for year of birth if blank
int getAge() {
    vector_char buf;
    print_string("Pls Enter Your Age : ", VGA_COLOR_BLUE);
    // For demo, just simulate user input
    input(buf.data); buf.sz = 0;
    int age = to_int(buf);

    // If input is empty or not a valid number, ask for year of birth
    if (buf.sz == 0 || age <= 0) {
        print_string("Age not provided. Please enter your year of birth : ", VGA_COLOR_BLUE);
        vector_char year_buf;
        input(year_buf.data); year_buf.sz = 0;
        int year_of_birth = to_int(year_buf);

        int current_year = get_current_year_from_rtc();

        // If year_of_birth is invalid, ask again (simple check)
        while (year_of_birth <= 1900 || year_of_birth > current_year) {
            print_string("Invalid year. Please enter a valid year of birth : ", VGA_COLOR_BLUE);
            year_buf.clear();
            input(year_buf.data); year_buf.sz = 0;
            year_of_birth = to_int(year_buf);
        }

        age = current_year - year_of_birth;
        print_string("Calculated your age as: ", VGA_COLOR_LIGHT_CYAN);
        print_string(vector_to_cstr(int_to_vector(age)), VGA_COLOR_LIGHT_CYAN);
        print_string("\n");
    }

    return age;
}

// ... other functions like getFirstName, getLastName, calculateLifeStage, etc. ...
vector_char to_vector(const char* str) {
    vector_char v;
    while (*str) v.push_back(*str++);
    return v;
}
vector_char concat(const vector_char* vecs, int count) {
    vector_char result;
    for (int i = 0; i < count; ++i)
        for (int j = 0; j < vecs[i].sz; ++j)
            result.push_back(vecs[i][j]);
    return result;
}

vector_char getFirstName() { return to_vector("Sora"); }
vector_char getLastName() { return to_vector("AI"); }
vector_char calculateLifeStage(int age) { 
    if (age < 13) return to_vector("child");
    if (age < 20) return to_vector("teenager");
    if (age < 65) return to_vector("adult");
    return to_vector("senior");
}
vector_char getMood() { return to_vector("happy"); }

void task1() {
    for (;;) {
        print_string("Task 1 running\n");
        yield();
    }
}

void task2() {
    for (;;) {
        print_string("Task 2 running\n");
        yield();
    }
}

extern "C" void kernel_main() {
    setup_paging(); // your paging setup
    init_tasking();
    create_task(task1);
    create_task(task2);
    while (1) {
        yield(); // scheduler runs tasks
    }
}