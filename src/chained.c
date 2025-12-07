/*
 * Chained Hash Map Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "chained.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Takes a key and capacity, returns bucket index
* Code adapted from Appleby, A. (2011). MurmurHash3 fmix32() finalizer. 
* Retrieved from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp.
* The bit-mixing sequence and associated constants in the hash_with_seed function 
* were adapted from the fmix32() finalizer of MurmurHash3.
*/ 
static size_t hash(int key, size_t capacity) {
    unsigned int k = (unsigned int)key;  // Convert to unsigned for bitwise ops
    k ^= (k >> 16);      // Mix high bits into low bits
    k *= 0x85ebca6b;     // Multiply by magic constant
    k ^= (k >> 13);      // More bit mixing
    k *= 0xc2b2ae35;     // Another magic constant
    k ^= (k >> 16);      // Final mix
    return k % capacity; // Reduce to valid bucket index
}

// Create a new chained hash map with specified capacity
ChainedHashMap* chained_create(size_t capacity) {
    // Allocate the main structure
    ChainedHashMap *map = malloc(sizeof(ChainedHashMap));
    if (!map) return NULL;  // Allocation failed
    
    // Allocate the bucket array
    map->buckets = calloc(capacity, sizeof(ChainedNode*));
    if (!map->buckets) {
        free(map);  // Clean up on failure
        return NULL;
    }
    
    map->capacity = capacity;  // Store capacity
    map->size = 0;             // Initially empty
    return map;                // Return the new map
}

// Free all memory used by the hash map
void chained_destroy(ChainedHashMap *map) {
    if (!map) return;  // Handle NULL input
    
    // Iterate through each bucket
    for (size_t i = 0; i < map->capacity; i++) {
        ChainedNode *node = map->buckets[i];
        // Free all nodes in this bucket's chain
        while (node) {
            ChainedNode *next = node->next;  // Save next before freeing
            free(node);
            node = next;
        }
    }
    free(map->buckets);  // Free the bucket array
    free(map);           // Free the main structure
}

// Insert or update a key value pair
bool chained_put(ChainedHashMap *map, int key, int value) {
    if (!map) return false;  // Handle NULL input
    
    size_t idx = hash(key, map->capacity);  // Find bucket
    ChainedNode *node = map->buckets[idx];
    
    // Search chain for existing key
    while (node) {
        if (node->key == key) {
            node->value = value;  // Update existing value
            return true;
        }
        node = node->next;
    }
    
    // Key not found, create new node
    ChainedNode *new_node = malloc(sizeof(ChainedNode));
    if (!new_node) return false;  // Allocation failed
    
    // Initialize new node
    new_node->key = key;
    new_node->value = value;
    new_node->next = map->buckets[idx];  // Insert at head of chain
    map->buckets[idx] = new_node;        // Update bucket head
    map->size++;                          // Increment count
    return true;
}

// Retrieve value for a key
bool chained_get(ChainedHashMap *map, int key, int *value) {
    if (!map) return false;  // Handle NULL input
    
    size_t idx = hash(key, map->capacity);  // Find bucket
    ChainedNode *node = map->buckets[idx];
    
    // Search chain for key
    while (node) {
        if (node->key == key) {
            if (value) *value = node->value;  // Return value if pointer provided
            return true;  // Found
        }
        node = node->next;
    }
    return false;  // Not found
}

// Delete a key from the map
bool chained_delete(ChainedHashMap *map, int key) {
    if (!map) return false;  // Handle NULL input
    
    size_t idx = hash(key, map->capacity);  // Find bucket
    ChainedNode *node = map->buckets[idx];
    ChainedNode *prev = NULL;  // Track previous node for unlinking
    
    // Search chain for key
    while (node) {
        if (node->key == key) {
            // Unlink node from chain
            if (prev) {
                prev->next = node->next;  // Bypass current node
            } else {
                map->buckets[idx] = node->next;  // Update bucket head
            }
            free(node);   // Free the node
            map->size--;  // Decrement count
            return true;
        }
        prev = node; // Update previous
        node = node->next; // Move to next node
    }
    return false;  // Key not found
}

// Return number of stored elements
size_t chained_size(ChainedHashMap *map) {
    return map ? map->size : 0;
}

// Calculate total memory usage in bytes
size_t chained_memory_usage(ChainedHashMap *map) {
    if (!map) return 0;
    size_t mem = sizeof(ChainedHashMap);              // Main struct
    mem += map->capacity * sizeof(ChainedNode*);      // Bucket array
    mem += map->size * sizeof(ChainedNode);           // All nodes
    return mem;
}

// Return the length of the longest chain in the hash map
int chained_max_chain_length(ChainedHashMap *map) {
    if (!map) return 0;
    
    int max_len = 0;
    // Check each bucket
    for (size_t i = 0; i < map->capacity; i++) {
        int len = 0;
        ChainedNode *node = map->buckets[i];
        // Count nodes in this chain
        while (node) {
            len++;
            node = node->next; // Move to next node
        }
        if (len > max_len) max_len = len;  // Update max
    }
    return max_len; // Return longest chain length
}