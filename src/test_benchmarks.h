/*
 * Benchmark Tests Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef TEST_BENCHMARKS_H
#define TEST_BENCHMARKS_H

#include <stddef.h>

// Benchmark result structure
typedef struct {
    double chained_ms;
    double linear_ms;
    double cuckoo_ms;
    int cuckoo_rehashes;
} BenchmarkResult;

// Run all benchmarks with specified parameters
void run_all_benchmarks(int test_size, size_t capacity);

// Individual benchmark functions
BenchmarkResult benchmark_insertion(int *keys, int n, size_t capacity);
BenchmarkResult benchmark_lookup(int *keys, int n, size_t capacity);
BenchmarkResult benchmark_deletion(int *keys, int n, size_t capacity);

// Analysis functions
void benchmark_memory(int n, size_t capacity);
void benchmark_worst_case_lookup(int n, size_t capacity);
void benchmark_scaling(void);

#endif