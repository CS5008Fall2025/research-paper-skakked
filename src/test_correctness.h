/*
 * Correctness Tests Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef TEST_CORRECTNESS_H
#define TEST_CORRECTNESS_H

#include <stdbool.h>

// Test result structure for tracking pass/fail counts
typedef struct {
    int passed;
    int total;
} TestResult;

// Run all correctness tests, returns combined results
TestResult run_all_correctness_tests(void);

// Individual test suites (return pass/total counts)
TestResult test_chained_correctness(void);
TestResult test_linear_correctness(void);
TestResult test_cuckoo_correctness(void);

// Stress tests with many elements
TestResult test_chained_stress(int n);
TestResult test_linear_stress(int n);
TestResult test_cuckoo_stress(int n);

#endif