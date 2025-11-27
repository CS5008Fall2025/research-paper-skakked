/*
 * Linear Probing Hash Map Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef LINEAR_PROBING_H
#define LINEAR_PROBING_H

#include <stdbool.h>
#include <stddef.h>

// Slot states for tracking entry status
typedef enum {
    EMPTY,      // Never used
    OCCUPIED,   // Contains valid key-value
    DELETED     // Was occupied, now deleted (tombstone)
} SlotState;

// Entry structure for each slot
typedef struct {
    int key;         // Key stored in slot
    int value;       // Value associated with key
    SlotState state; // Current state of slot
} LinearEntry;

// Main hash map structure
typedef struct {
    LinearEntry *entries;  // Array of entries
    size_t capacity;       // Total number of slots
    size_t size;           // Number of occupied slots
} LinearHashMap;

LinearHashMap* linear_create(size_t capacity); // Create a new linear probing hash map
void linear_destroy(LinearHashMap *map); // Destroy the hash map and free memory
bool linear_put(LinearHashMap *map, int key, int value); // Insert or update a key value pair
bool linear_get(LinearHashMap *map, int key, int *value); // Retrieve value for key value pair
bool linear_delete(LinearHashMap *map, int key); // Delete a key value pair
size_t linear_size(LinearHashMap *map); // Get number of stored elements
size_t linear_memory_usage(LinearHashMap *map); // Get total memory usage in bytes
int linear_probe_count(LinearHashMap *map, int key); // Count probes needed to find or miss a key

#endif