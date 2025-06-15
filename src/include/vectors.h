#ifndef VECTORS_H
#define VECTORS_H

#include <stddef.h> // For size_t
#include <string.h>
// Since we're in freestanding mode, we can't use std::vector.
// We'll provide a simple string concatenation function for C-style strings.
// This is a very basic implementation and doesn't handle all cases.

// Concatenates multiple C-style strings into a single buffer.
// Requires the caller to provide a buffer large enough to hold the result.
// Returns the length of the concatenated string.
size_t concat_strings(char* dest, size_t dest_size, const char* strs[], size_t num_strs) {
    size_t total_len = 0;
    for (size_t i = 0; i < num_strs; ++i) {
        total_len += strlen(strs[i]);
    }

    if (total_len >= dest_size) {
        // Handle error: destination buffer too small
        return 0; // Or some error code
    }

    size_t current_pos = 0;
    for (size_t i = 0; i < num_strs; ++i) {
        const char* str = strs[i];
        size_t len = strlen(str);
        for (size_t j = 0; j < len; ++j) {
            dest[current_pos++] = str[j];
        }
    }
    dest[current_pos] = '\0'; // Null-terminate the string
    return current_pos;
}

#endif