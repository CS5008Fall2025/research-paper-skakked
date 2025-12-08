/*
 * Test Utilities Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef TEST_UTILS_H // Include guard
#define TEST_UTILS_H // Prevent multiple inclusions

#include <stddef.h>

// Default test parameters
#define DEFAULT_TEST_SIZE 10000   // Number of elements to test with
#define DEFAULT_CAPACITY 20000    // Initial capacity for hash maps

// Get current time in milliseconds for benchmarking
double get_time_ms(void);

// Generate array of n random keys
// Caller is responsible for freeing returned array
int* generate_random_keys(int n);

// Generate array of sequential keys (0, 1, 2, ...)
// Caller is responsible for freeing returned array
int* generate_sequential_keys(int n);

// Print a section header for test output
void print_section_header(const char *title);

// Print a subsection header
void print_subsection(const char *title);

#endif