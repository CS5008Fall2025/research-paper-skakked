/*
 * Benchmark Tests Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "test_benchmarks.h"
#include "test_utils.h"
#include "chained.h"
#include "linear_probing.h"
#include "cuckoo.h"
#include <stdio.h>
#include <stdlib.h>

// Benchmark insertion performance
BenchmarkResult benchmark_insertion(int *keys, int n, size_t capacity) {
    BenchmarkResult result = {0, 0, 0, 0};
    double start, end;
    
    // Benchmark Chained HashMap insertion
    ChainedHashMap *ch = chained_create(capacity);
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        chained_put(ch, keys[i], i);
    }
    end = get_time_ms();
    result.chained_ms = end - start;
    chained_destroy(ch);
    
    // Benchmark Linear Probing insertion
    LinearHashMap *lh = linear_create(capacity);
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        linear_put(lh, keys[i], i);
    }
    end = get_time_ms();
    result.linear_ms = end - start;
    linear_destroy(lh);
    
    // Benchmark Cuckoo insertion
    CuckooHashMap *cu = cuckoo_create(capacity);
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        cuckoo_put(cu, keys[i], i);
    }
    end = get_time_ms();
    result.cuckoo_ms = end - start;
    result.cuckoo_rehashes = cuckoo_rehash_count(cu);
    cuckoo_destroy(cu);
    
    return result;
}

// Benchmark lookup performance
BenchmarkResult benchmark_lookup(int *keys, int n, size_t capacity) {
    BenchmarkResult result = {0, 0, 0, 0};
    double start, end;
    int val;
    
    // Build all tables with the same data first
    ChainedHashMap *ch = chained_create(capacity);
    LinearHashMap *lh = linear_create(capacity);
    CuckooHashMap *cu = cuckoo_create(capacity);
    
    for (int i = 0; i < n; i++) {
        chained_put(ch, keys[i], i);
        linear_put(lh, keys[i], i);
        cuckoo_put(cu, keys[i], i);
    }
    
    // Benchmark Chained lookups
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        chained_get(ch, keys[i], &val);
    }
    end = get_time_ms();
    result.chained_ms = end - start;
    
    // Benchmark Linear Probing lookups
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        linear_get(lh, keys[i], &val);
    }
    end = get_time_ms();
    result.linear_ms = end - start;
    
    // Benchmark Cuckoo lookups
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        cuckoo_get(cu, keys[i], &val);
    }
    end = get_time_ms();
    result.cuckoo_ms = end - start;
    result.cuckoo_rehashes = cuckoo_rehash_count(cu);
    
    chained_destroy(ch);
    linear_destroy(lh);
    cuckoo_destroy(cu);
    
    return result;
}

