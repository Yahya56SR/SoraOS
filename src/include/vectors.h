#ifndef VECTORS_H
#define VECTORS_H

#include <stddef.h> // For size_t

// Function to concatenate multiple strings into a single buffer
size_t concat_strings(char* dest, size_t dest_size, const char* src[], size_t num_src);

#endif