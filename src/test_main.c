/*
 * Test Suite Main Entry Point
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "test_utils.h"
#include "test_correctness.h"
#include "test_benchmarks.h"

// Print usage information
static void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n\n", program_name);
    printf("Options:\n");
    printf("  --all         Run all tests and benchmarks (default)\n");
    printf("  --correctness Run only correctness tests\n");
    printf("  --benchmarks  Run only benchmark tests\n");
    printf("  --size N      Set test size (default: %d)\n", DEFAULT_TEST_SIZE);
    printf("  --capacity N  Set initial capacity (default: %d)\n", DEFAULT_CAPACITY);
    printf("  --help        Show this help message\n");
}

int main(int argc, char *argv[]) {
    // Default options
    int run_correctness = 1;
    int run_benchmarks = 1;
    int test_size = DEFAULT_TEST_SIZE;
    size_t capacity = DEFAULT_CAPACITY;
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--all") == 0) {
            run_correctness = 1;
            run_benchmarks = 1;
        } else if (strcmp(argv[i], "--correctness") == 0) {
            run_correctness = 1;
            run_benchmarks = 0;
        } else if (strcmp(argv[i], "--benchmarks") == 0) {
            run_correctness = 0;
            run_benchmarks = 1;
        } else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            test_size = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--capacity") == 0 && i + 1 < argc) {
            capacity = (size_t)atoi(argv[++i]);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    
    // Print header
    printf("========================================\n");
    printf("   Hash Map Implementation Test Suite\n");
    printf("========================================\n");
    printf("Test size: %d | Capacity: %zu\n", test_size, capacity);
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Run selected tests
    if (run_correctness) {
        TestResult result = run_all_correctness_tests();
        if (result.passed != result.total) {
            printf("\n*** WARNING: Some correctness tests failed! ***\n");
        }
    }
    
    if (run_benchmarks) {
        print_section_header("PERFORMANCE BENCHMARKS");
        run_all_benchmarks(test_size, capacity);
    }
    
    // Print footer
    printf("\n========================================\n");
    printf("   Test Suite Complete\n");
    printf("========================================\n");
    
    return 0;
}
