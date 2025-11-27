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

typedef enum { // Slot states for linear probing
    EMPTY,
    OCCUPIED,
    DELETED
} SlotState;

typedef struct { // Entry in linear probing hash map
    int key;
    int value;
    SlotState state;
} LinearEntry;

typedef struct { // Linear Probing Hash Map structure
    LinearEntry *entries;
    size_t capacity;
    size_t size;
} LinearHashMap;

/* Create a new linear probing hash map with given capacity */
LinearHashMap* linear_create(size_t capacity);

/* Destroy and free the hash map */
void linear_destroy(LinearHashMap *map);

/* Insert or update a key-value pair */
bool linear_put(LinearHashMap *map, int key, int value);

/* Retrieve value for key. Returns true if found. */
bool linear_get(LinearHashMap *map, int key, int *value);

/* Delete a key. Returns true if key existed. */
bool linear_delete(LinearHashMap *map, int key);

/* Get number of stored elements */
size_t linear_size(LinearHashMap *map);

/* Get total memory usage in bytes */
size_t linear_memory_usage(LinearHashMap *map);

/* Count probes needed to find or miss a key */
int linear_probe_count(LinearHashMap *map, int key);

#endif