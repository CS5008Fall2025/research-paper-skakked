/*
 * Linear Probing Hash Map Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "linear_probing.h"
#include <stdlib.h>

/* Hash function using MurmurHash-inspired bit mixing
* Code adapted from Appleby, A. (2011). MurmurHash3 fmix32() finalizer. 
* Retrieved from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp.
* The bit-mixing sequence and associated constants in the hash_with_seed function 
* were adapted from the fmix32() finalizer of MurmurHash3.
*/
static size_t hash(int key, size_t capacity) {
    unsigned int k = (unsigned int)key;
    k ^= (k >> 16);      // Mix high bits down
    k *= 0x85ebca6b;     // Multiply by magic constant
    k ^= (k >> 13);      // More mixing
    k *= 0xc2b2ae35;     // Another constant
    k ^= (k >> 16);      // Final mix
    return k % capacity; // Reduce to valid index
}

// Create a new linear probing hash map
LinearHashMap* linear_create(size_t capacity) {
    // Allocate main structure
    LinearHashMap *map = malloc(sizeof(LinearHashMap));
    if (!map) return NULL;
    
    // Allocate entry array
    map->entries = calloc(capacity, sizeof(LinearEntry));
    if (!map->entries) {
        free(map);
        return NULL;
    }
    
    // Initialize all slots as EMPTY
    for (size_t i = 0; i < capacity; i++) {
        map->entries[i].state = EMPTY;
    }
    
    map->capacity = capacity;
    map->size = 0;
    return map;
}

// Free all memory
void linear_destroy(LinearHashMap *map) {
    if (!map) return;
    free(map->entries);  // Free entry array
    free(map);           // Free main struct
}

// Insert or update a key-value pair
bool linear_put(LinearHashMap *map, int key, int value) {
    if (!map || map->size >= map->capacity) return false;  // Full or NULL
    
    size_t idx = hash(key, map->capacity);  // Starting index
    size_t start = idx;                      // Remember start to detect full loop
    
    do {
        // Found empty or deleted slot
        // Insert new key-value
        if (map->entries[idx].state == EMPTY || 
            map->entries[idx].state == DELETED) {
            map->entries[idx].key = key;
            map->entries[idx].value = value;
            map->entries[idx].state = OCCUPIED;
            map->size++;
            return true;
        }
        // Found existing key
        // Update value
        if (map->entries[idx].state == OCCUPIED && 
            map->entries[idx].key == key) {
            map->entries[idx].value = value;
            return true;  // No size change, just update
        }
        // Linear probe: move to next slot
        idx = (idx + 1) % map->capacity;
    } while (idx != start);  // Stop if we've checked all slots
    
    return false;  // Table is full
}

// Retrieve value for a key
bool linear_get(LinearHashMap *map, int key, int *value) {
    if (!map) return false;
    
    size_t idx = hash(key, map->capacity);  // Starting index
    size_t start = idx;
    
    do {
        // EMPTY means key was never here
        if (map->entries[idx].state == EMPTY) {
            return false;
        }
        // Check if this slot has our key
        if (map->entries[idx].state == OCCUPIED && 
            map->entries[idx].key == key) {
            if (value) *value = map->entries[idx].value;
            return true;
        }
        // Continue probing
        // Linear probe
        // Move to next slot
        idx = (idx + 1) % map->capacity;
    } while (idx != start);
    
    return false;  // Not found
}

// Delete a key
// Marks the slot as DELETED
bool linear_delete(LinearHashMap *map, int key) {
    if (!map) return false;
    
    size_t idx = hash(key, map->capacity);
    size_t start = idx;
    
    do {
        // EMPTY means key was never here
        if (map->entries[idx].state == EMPTY) {
            return false;
        }
        // Found the key 
        // Mark as deleted
        if (map->entries[idx].state == OCCUPIED && 
            map->entries[idx].key == key) {
            map->entries[idx].state = DELETED;  // Tombstone, not EMPTY
            map->size--;
            return true;
        }
        idx = (idx + 1) % map->capacity;
    } while (idx != start);
    
    return false;  // Key not found
}

// Return number of stored elements
size_t linear_size(LinearHashMap *map) {
    return map ? map->size : 0;
}

// Calculate total memory usage
size_t linear_memory_usage(LinearHashMap *map) {
    if (!map) return 0;
    // Main struct + all entries
    // No additional dynamic allocations per entry
    return sizeof(LinearHashMap) + map->capacity * sizeof(LinearEntry);
}

// Count probes needed to find or determine absence of key
int linear_probe_count(LinearHashMap *map, int key) {
    if (!map) return 0;
    
    size_t idx = hash(key, map->capacity);
    size_t start = idx;
    int probes = 0; // Probe counter
    
    do {
        probes++;  // Count this probe
        // EMPTY ends the search
        if (map->entries[idx].state == EMPTY) {
            return probes;
        }
        // Found the key
        if (map->entries[idx].state == OCCUPIED && 
            map->entries[idx].key == key) {
            return probes;
        }
        idx = (idx + 1) % map->capacity; // Next slot
    } while (idx != start); // Full loop
    
    return probes;  // Searched entire table
}