/*
 * Cuckoo Hash Map Header
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#ifndef CUCKOO_H
#define CUCKOO_H

#include <stdbool.h>
#include <stddef.h>

// Entry structure for cuckoo hash slots
typedef struct {
    int key;        // Key stored in slot
    int value;      // Value associated with key
    bool occupied;  // Whether slot contains valid data
} CuckooEntry;

// Main cuckoo hash map structure
typedef struct {
    CuckooEntry *table1;   // First hash table
    CuckooEntry *table2;   // Second hash table
    size_t capacity;       // Capacity of EACH table
    size_t size;           // Total elements across both tables
    unsigned int seed1;    // Seed for first hash function
    unsigned int seed2;    // Seed for second hash function
    int rehash_count;      // Number of rehashes performed
} CuckooHashMap;

CuckooHashMap* cuckoo_create(size_t capacity); // Initialize cuckoo hash map
void cuckoo_destroy(CuckooHashMap *map); // Free resources used by cuckoo hash map
bool cuckoo_put(CuckooHashMap *map, int key, int value); // Insert key value pair
bool cuckoo_get(CuckooHashMap *map, int key, int *value); // Retrieve value for key
bool cuckoo_delete(CuckooHashMap *map, int key); // Remove key-value pair
size_t cuckoo_size(CuckooHashMap *map); // Get number of elements in map
size_t cuckoo_memory_usage(CuckooHashMap *map); // Get total memory usage in bytes
int cuckoo_rehash_count(CuckooHashMap *map); // Get number of rehashes performed
double cuckoo_load_factor(CuckooHashMap *map); // Get current load factor

#endif