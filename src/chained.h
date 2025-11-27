/*
 * Chained Hash Map Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
*/

#ifndef CHAINED_H
#define CHAINED_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ChainedNode { // Node in the linked list for chaining
    int key;
    int value;
    struct ChainedNode *next;
} ChainedNode;

typedef struct { // Chained Hash Map structure
    ChainedNode **buckets;
    size_t capacity;
    size_t size;
} ChainedHashMap;

/* Create a new chained hash map with given capacity */
ChainedHashMap* chained_create(size_t capacity);

/* Destroy and free the hash map */
void chained_destroy(ChainedHashMap *map);

/* Insert or update a key-value pair */
bool chained_put(ChainedHashMap *map, int key, int value);

/* Retrieve value for key. Returns true if found. */
bool chained_get(ChainedHashMap *map, int key, int *value);

/* Delete a key. Returns true if key existed. */
bool chained_delete(ChainedHashMap *map, int key);

/* Get number of stored elements */
size_t chained_size(ChainedHashMap *map);

/* Get total memory usage in bytes */
size_t chained_memory_usage(ChainedHashMap *map);

/* Get length of longest chain */
int chained_max_chain_length(ChainedHashMap *map);

#endif