/*
 * Test Utilities Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "test_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Get current time in milliseconds for benchmarking
double get_time_ms(void) {
    return (double)clock() / CLOCKS_PER_SEC * 1000.0;
}

// Generate array of n random keys
int* generate_random_keys(int n) {
    int *keys = malloc(n * sizeof(int));
    if (!keys) return NULL;  // Handle allocation failure
    
    for (int i = 0; i < n; i++) {
        keys[i] = rand();  // Random integer
    }
    return keys;
}

// Generate array of sequential keys (0, 1, 2, ...)
int* generate_sequential_keys(int n) {
    int *keys = malloc(n * sizeof(int));
    if (!keys) return NULL;  // Handle allocation failure
    
    for (int i = 0; i < n; i++) { // Sequential integers
        keys[i] = i;
    }
    return keys;
}

// Print a section header for test output
void print_section_header(const char *title) {
    printf("\n=== %s ===\n\n", title);
}

// Print a subsection header
void print_subsection(const char *title) {
    printf("\n--- %s ---\n", title);
}