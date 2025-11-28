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

// Benchmark deletion performance
BenchmarkResult benchmark_deletion(int *keys, int n, size_t capacity) {
    BenchmarkResult result = {0, 0, 0, 0};
    double start, end;
    
    // Build all tables first
    ChainedHashMap *ch = chained_create(capacity);
    LinearHashMap *lh = linear_create(capacity);
    CuckooHashMap *cu = cuckoo_create(capacity);
    
    for (int i = 0; i < n; i++) {
        chained_put(ch, keys[i], i);
        linear_put(lh, keys[i], i);
        cuckoo_put(cu, keys[i], i);
    }
    
    // Benchmark Chained deletions
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        chained_delete(ch, keys[i]);
    }
    end = get_time_ms();
    result.chained_ms = end - start;
    
    // Benchmark Linear Probing deletions
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        linear_delete(lh, keys[i]);
    }
    end = get_time_ms();
    result.linear_ms = end - start;
    
    // Benchmark Cuckoo deletions
    start = get_time_ms();
    for (int i = 0; i < n; i++) {
        cuckoo_delete(cu, keys[i]);
    }
    end = get_time_ms();
    result.cuckoo_ms = end - start;
    
    chained_destroy(ch);
    linear_destroy(lh);
    cuckoo_destroy(cu);
    
    return result;
}

// Compare memory usage across implementations
void benchmark_memory(int n, size_t capacity) {
    print_section_header("MEMORY USAGE");
    
    int *keys = generate_random_keys(n);
    
    // Create and fill all maps
    ChainedHashMap *ch = chained_create(capacity);
    LinearHashMap *lh = linear_create(capacity);
    CuckooHashMap *cu = cuckoo_create(capacity);
    
    for (int i = 0; i < n; i++) {
        chained_put(ch, keys[i], i);
        linear_put(lh, keys[i], i);
        cuckoo_put(cu, keys[i], i);
    }
    
    // Report memory usage and structure-specific metrics
    printf("Elements stored: %d\n\n", n);
    printf("Chained:        %zu bytes (max chain: %d)\n", 
           chained_memory_usage(ch), chained_max_chain_length(ch));
    printf("Linear Probing: %zu bytes\n", 
           linear_memory_usage(lh));
    printf("Cuckoo:         %zu bytes (load: %.2f%%)\n", 
           cuckoo_memory_usage(cu), cuckoo_load_factor(cu) * 100);
    
    chained_destroy(ch);
    linear_destroy(lh);
    cuckoo_destroy(cu);
    free(keys);
}

// Analyze worst-case lookup behavior
void benchmark_worst_case_lookup(int n, size_t capacity) {
    print_section_header("WORST-CASE LOOKUP ANALYSIS");
    
    int *keys = generate_random_keys(n);
    
    // Create and fill all maps
    ChainedHashMap *ch = chained_create(capacity);
    LinearHashMap *lh = linear_create(capacity);
    CuckooHashMap *cu = cuckoo_create(capacity);
    
    for (int i = 0; i < n; i++) {
        chained_put(ch, keys[i], i);
        linear_put(lh, keys[i], i);
        cuckoo_put(cu, keys[i], i);
    }
    
    // Find worst-case probes for linear probing
    int max_probes = 0;
    for (int i = 0; i < n; i++) {
        int probes = linear_probe_count(lh, keys[i]);
        if (probes > max_probes) max_probes = probes;
    }
    
    // Report worst-case metrics
    printf("Chained max chain length:  %d\n", chained_max_chain_length(ch));
    printf("Linear probing max probes: %d\n", max_probes);
    printf("Cuckoo max lookups:        2 (guaranteed)\n");
    
    chained_destroy(ch);
    linear_destroy(lh);
    cuckoo_destroy(cu);
    free(keys);
}

// Test scaling behavior with increasing sizes
void benchmark_scaling(void) {
    print_section_header("SCALING ANALYSIS");
    
    int sizes[] = {1000, 5000, 10000, 25000, 50000, 75000, 100000, 125000, 150000, 175000, 200000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    printf("%-10s | %-12s | %-12s | %-12s\n", 
           "Size", "Chained", "Linear", "Cuckoo");
    printf("-----------|--------------|--------------|-------------\n");
    
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        size_t capacity = n * 2;
        int *keys = generate_random_keys(n);
        
        BenchmarkResult r = benchmark_insertion(keys, n, capacity);
        
        printf("%-10d | %10.3f ms | %10.3f ms | %10.3f ms\n",
               n, r.chained_ms, r.linear_ms, r.cuckoo_ms);
        
        free(keys);
    }
}

// Print benchmark results in formatted table
static void print_benchmark_results(BenchmarkResult r) {
    printf("Chained:        %.3f ms\n", r.chained_ms);
    printf("Linear Probing: %.3f ms\n", r.linear_ms);
    printf("Cuckoo:         %.3f ms", r.cuckoo_ms);
    if (r.cuckoo_rehashes > 0) {
        printf(" (rehashes: %d)", r.cuckoo_rehashes);
    }
    printf("\n");
}

// Run all benchmarks
void run_all_benchmarks(int test_size, size_t capacity) {
    BenchmarkResult r;
    
    // Random keys benchmarks
    print_subsection("Random Keys");
    int *random_keys = generate_random_keys(test_size);
    
    printf("\nINSERTION (%d elements):\n", test_size);
    r = benchmark_insertion(random_keys, test_size, capacity);
    print_benchmark_results(r);
    
    printf("\nLOOKUP (%d lookups):\n", test_size);
    r = benchmark_lookup(random_keys, test_size, capacity);
    print_benchmark_results(r);
    
    printf("\nDELETION (%d deletions):\n", test_size);
    r = benchmark_deletion(random_keys, test_size, capacity);
    print_benchmark_results(r);
    
    free(random_keys);
    
    // Sequential keys benchmarks (reveals clustering issues)
    print_subsection("Sequential Keys");
    int *seq_keys = generate_sequential_keys(test_size);
    
    printf("\nINSERTION (%d elements):\n", test_size);
    r = benchmark_insertion(seq_keys, test_size, capacity);
    print_benchmark_results(r);
    
    printf("\nLOOKUP (%d lookups):\n", test_size);
    r = benchmark_lookup(seq_keys, test_size, capacity);
    print_benchmark_results(r);
    
    free(seq_keys);
    
    // Memory and worst-case analysis
    benchmark_memory(test_size, capacity);
    benchmark_worst_case_lookup(test_size, capacity);
    benchmark_scaling();
}