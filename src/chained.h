/*
 * Chained Hash Map Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef CHAINED_H // Include guard
#define CHAINED_H 

#include <stdbool.h>
#include <stddef.h>

// Node structure for linked list in each bucket
typedef struct ChainedNode {
    int key;                    // Key stored in this node
    int value;                  // Value associated with key
    struct ChainedNode *next;   // Pointer to next node in chain
} ChainedNode;

// Main hash map structure
typedef struct {
    ChainedNode **buckets;  // Array of pointers to linked list heads
    size_t capacity;        // Number of buckets
    size_t size;            // Number of key-value pairs stored
} ChainedHashMap;

ChainedHashMap* chained_create(size_t capacity); // Create a new chained hash map
void chained_destroy(ChainedHashMap *map); // Destroy the hash map and free memory
bool chained_put(ChainedHashMap *map, int key, int value); // Insert or update a key-value pair
bool chained_get(ChainedHashMap *map, int key, int *value); // Retrieve value for key. Returns true if found
bool chained_delete(ChainedHashMap *map, int key); // Delete a key value pair
size_t chained_size(ChainedHashMap *map); // Get number of stored elements
size_t chained_memory_usage(ChainedHashMap *map); // Get total memory usage in bytes
int chained_max_chain_length(ChainedHashMap *map); // Get length of longest chain

#endif