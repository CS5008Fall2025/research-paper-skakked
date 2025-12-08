/*
 * Correctness Tests Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "test_correctness.h"
#include "test_utils.h"
#include "chained.h"
#include "linear_probing.h"
#include "cuckoo.h"
#include <stdio.h>
#include <stdlib.h>

// Helper macro for test assertions
// Increments total tests and passed tests if condition is true
#define TEST_ASSERT(result, condition) \ 
    do { \ 
        (result).total++; \
        if (condition) (result).passed++; \
    } while(0)

// Test chained hash map basic operations
TestResult test_chained_correctness(void) {
    TestResult result = {0, 0}; // Initialize test result
    int val;
    
    printf("Testing Chained HashMap...\n");
    ChainedHashMap *map = chained_create(100); // Create map with capacity 100
    
    // Test 1: Insert and retrieve
    chained_put(map, 42, 100); // Insert key 42 with value 100
    TEST_ASSERT(result, chained_get(map, 42, &val) && val == 100);
    
    // Test 2: Update existing key
    chained_put(map, 42, 200); // Update key 42 with new value 200
    TEST_ASSERT(result, chained_get(map, 42, &val) && val == 200);
    
    // Test 3: Delete key
    chained_delete(map, 42); // Delete key 42
    TEST_ASSERT(result, !chained_get(map, 42, &val));
    
    // Test 4: Miss on non-existent key
    TEST_ASSERT(result, !chained_get(map, 999, &val));
    
    // Test 5: Multiple inserts
    for (int i = 0; i < 50; i++) { // Insert keys 0-49
        chained_put(map, i, i * 10);
    }
    TEST_ASSERT(result, chained_size(map) == 50);
    
    // Test 6: Retrieve after multiple inserts
    TEST_ASSERT(result, chained_get(map, 25, &val) && val == 250);
    
    // Test 7: Delete from middle
    chained_delete(map, 25);
    TEST_ASSERT(result, !chained_get(map, 25, &val));
    TEST_ASSERT(result, chained_size(map) == 49);
    
    chained_destroy(map);
    printf("  Chained: %d/%d tests passed\n", result.passed, result.total);
    return result;
}

// Test linear probing hash map basic operations
TestResult test_linear_correctness(void) {
    TestResult result = {0, 0};
    int val;
    
    printf("Testing Linear Probing HashMap...\n");
    LinearHashMap *map = linear_create(100);
    
    // Test 1: Insert and retrieve
    linear_put(map, 42, 100);
    TEST_ASSERT(result, linear_get(map, 42, &val) && val == 100);
    
    // Test 2: Update existing key
    linear_put(map, 42, 200);
    TEST_ASSERT(result, linear_get(map, 42, &val) && val == 200);
    
    // Test 3: Delete key (tombstone)
    linear_delete(map, 42);
    TEST_ASSERT(result, !linear_get(map, 42, &val));
    
    // Test 4: Miss on non-existent key
    TEST_ASSERT(result, !linear_get(map, 999, &val));
    
    // Test 5: Insert after delete (reuse tombstone)
    linear_put(map, 42, 300);
    TEST_ASSERT(result, linear_get(map, 42, &val) && val == 300);
    
    // Test 6: Multiple inserts with potential collisions
    for (int i = 0; i < 50; i++) {
        linear_put(map, i * 100, i);  // Spread keys to test probing
    }
    TEST_ASSERT(result, linear_size(map) == 51);  // 50 + key 42
    
    // Test 7: Probe count is reasonable
    int probes = linear_probe_count(map, 42);
    TEST_ASSERT(result, probes > 0 && probes <= 50);
    
    linear_destroy(map);
    printf("  Linear Probing: %d/%d tests passed\n", result.passed, result.total);
    return result;
}

// Test cuckoo hash map basic operations
TestResult test_cuckoo_correctness(void) {
    TestResult result = {0, 0};
    int val;
    
    printf("Testing Cuckoo HashMap...\n");
    CuckooHashMap *map = cuckoo_create(100);
    
    // Test 1: Insert and retrieve
    cuckoo_put(map, 42, 100);
    TEST_ASSERT(result, cuckoo_get(map, 42, &val) && val == 100);
    
    // Test 2: Update existing key
    cuckoo_put(map, 42, 200);
    TEST_ASSERT(result, cuckoo_get(map, 42, &val) && val == 200);
    
    // Test 3: Delete key
    cuckoo_delete(map, 42);
    TEST_ASSERT(result, !cuckoo_get(map, 42, &val));
    
    // Test 4: Miss on non-existent key
    TEST_ASSERT(result, !cuckoo_get(map, 999, &val));
    
    // Test 5: Multiple inserts (may trigger displacement)
    for (int i = 0; i < 50; i++) {
        cuckoo_put(map, i, i * 10);
    }
    TEST_ASSERT(result, cuckoo_size(map) == 50);
    
    // Test 6: All inserted keys retrievable
    bool all_found = true;
    for (int i = 0; i < 50; i++) {
        if (!cuckoo_get(map, i, &val) || val != i * 10) {
            all_found = false;
            break;
        }
    }
    TEST_ASSERT(result, all_found);
    
    // Test 7: Load factor is reasonable
    TEST_ASSERT(result, cuckoo_load_factor(map) > 0 && cuckoo_load_factor(map) < 1);
    
    cuckoo_destroy(map);
    printf("  Cuckoo: %d/%d tests passed\n", result.passed, result.total);
    return result;
}

// Stress test chained hash map with n elements
TestResult test_chained_stress(int n) {
    TestResult result = {0, 0};
    
    printf("Stress testing Chained HashMap with %d elements...\n", n);
    ChainedHashMap *map = chained_create(n / 2);  // Force collisions
    int *keys = generate_random_keys(n);
    
    // Insert all keys
    for (int i = 0; i < n; i++) {
        chained_put(map, keys[i], i);
    }
    
    // Verify size (may be less due to duplicate random keys)
    TEST_ASSERT(result, chained_size(map) <= (size_t)n);
    
    // Verify all keys retrievable
    int val;
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (chained_get(map, keys[i], &val)) found++;
    }
    TEST_ASSERT(result, found == n);  // All lookups should succeed
    
    chained_destroy(map);
    free(keys);
    printf("  Stress: %d/%d tests passed\n", result.passed, result.total);
    return result; // Return test results
}

// Stress test linear probing hash map with n elements
TestResult test_linear_stress(int n) {
    TestResult result = {0, 0};
    
    printf("Stress testing Linear Probing HashMap with %d elements...\n", n);
    LinearHashMap *map = linear_create(n * 2);  // 50% load factor
    int *keys = generate_random_keys(n);
    
    // Insert all keys
    for (int i = 0; i < n; i++) {
        linear_put(map, keys[i], i);
    }
    
    TEST_ASSERT(result, linear_size(map) <= (size_t)n);
    
    // Verify all keys retrievable
    int val;
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (linear_get(map, keys[i], &val)) found++;
    }
    TEST_ASSERT(result, found == n);
    
    linear_destroy(map);
    free(keys);
    printf("  Stress: %d/%d tests passed\n", result.passed, result.total);
    return result;
}

// Stress test cuckoo hash map with n elements
TestResult test_cuckoo_stress(int n) {
    TestResult result = {0, 0};
    
    printf("Stress testing Cuckoo HashMap with %d elements...\n", n);
    CuckooHashMap *map = cuckoo_create(n);  // Will auto-expand if needed
    int *keys = generate_random_keys(n);
    
    // Insert all keys
    for (int i = 0; i < n; i++) {
        cuckoo_put(map, keys[i], i);
    }
    
    TEST_ASSERT(result, cuckoo_size(map) <= (size_t)n);
    
    // Verify all keys retrievable
    int val; // Temporary variable for retrieved value
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (cuckoo_get(map, keys[i], &val)) found++;
    }
    TEST_ASSERT(result, found == n);
    
    // Check rehash count is reasonable
    TEST_ASSERT(result, cuckoo_rehash_count(map) < 10);
    
    cuckoo_destroy(map);
    free(keys);
    printf("  Stress: %d/%d tests passed (rehashes: %d)\n", 
           result.passed, result.total, cuckoo_rehash_count(map));
    return result;
}

// Run all correctness tests
TestResult run_all_correctness_tests(void) {
    TestResult total = {0, 0};
    TestResult r;
    
    print_section_header("CORRECTNESS TESTS");
    
    // Basic tests
    r = test_chained_correctness();
    total.passed += r.passed; total.total += r.total;
    // Linear probing tests
    r = test_linear_correctness();
    total.passed += r.passed; total.total += r.total;
    // Cuckoo tests
    r = test_cuckoo_correctness();
    total.passed += r.passed; total.total += r.total;
    
    // Stress tests
    print_subsection("Stress Tests");
    // Define default test size
    r = test_chained_stress(DEFAULT_TEST_SIZE);
    total.passed += r.passed; total.total += r.total;
    // Linear probing stress test
    r = test_linear_stress(DEFAULT_TEST_SIZE);
    total.passed += r.passed; total.total += r.total;
    // Cuckoo stress test
    r = test_cuckoo_stress(DEFAULT_TEST_SIZE);
    total.passed += r.passed; total.total += r.total;
    // Summary
    printf("\nTotal: %d/%d correctness tests passed\n", total.passed, total.total);
    return total;
}