/*
 * Cuckoo Hash Map Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 * 
 * Cuckoo Hash Map Implementation is based on: Pagh, R., & Rodler, F. F. (2004). Cuckoo hashing. 
 * Journal of Algorithms, 51(2), 122-144.
 * 
 */

#include "cuckoo.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Maximum displacement attempts before triggering rehash
#define MAX_DISPLACEMENTS 500

/* Hash function with configurable seed for generating independent h1 and h2
* Code adapted from Appleby, A. (2011). MurmurHash3 fmix32() finalizer. 
* Retrieved from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp.
* The bit-mixing sequence and associated constants in the hash_with_seed function 
* were adapted from the fmix32() finalizer of MurmurHash3.
*/ 
static size_t hash_with_seed(int key, unsigned int seed, size_t capacity) {
    unsigned int k = (unsigned int)key;
    k ^= seed;           // Mix in the seed
    k ^= (k >> 16);      // Bit mixing
    k *= 0x85ebca6b;
    k ^= (k >> 13);
    k *= 0xc2b2ae35;
    k ^= (k >> 16);
    return k % capacity;
}

// First hash function - uses seed1
static size_t h1(CuckooHashMap *map, int key) {
    return hash_with_seed(key, map->seed1, map->capacity);
}

// Second hash function
// Uses seed2 to ensure independence from h1
// Different seed produces different hash outputs
static size_t h2(CuckooHashMap *map, int key) {
    return hash_with_seed(key, map->seed2, map->capacity);
}

// Initialize hash function seeds with random values
static void init_seeds(CuckooHashMap *map) {
    map->seed1 = (unsigned int)rand();
    map->seed2 = (unsigned int)rand();
    // Ensure seeds are different for independent hash functions
    while (map->seed2 == map->seed1) {
        map->seed2 = (unsigned int)rand();
    }
}

// Create a new cuckoo hash map
CuckooHashMap* cuckoo_create(size_t capacity) {
    // Allocate main structure
    CuckooHashMap *map = malloc(sizeof(CuckooHashMap));
    if (!map) return NULL;
    
    // Allocate both tables
    map->table1 = calloc(capacity, sizeof(CuckooEntry));
    map->table2 = calloc(capacity, sizeof(CuckooEntry));
    
    // Check for allocation failure
    if (!map->table1 || !map->table2) {
        free(map->table1);
        free(map->table2);
        free(map);
        return NULL;
    }
    
    map->capacity = capacity;    // Each table has this capacity
    map->size = 0;               // Initially empty
    map->rehash_count = 0;       // No rehashes yet
    
    srand((unsigned int)time(NULL));  // Seed random number generator
    init_seeds(map);                   // Generate hash function seeds
    
    return map;
}

// Free all memory
void cuckoo_destroy(CuckooHashMap *map) {
    if (!map) return;
    free(map->table1);
    free(map->table2);
    free(map);
}

// Forward declaration for mutual recursion with insert
static bool cuckoo_rehash(CuckooHashMap *map);

// Internal insert function
// allow_rehash controls whether we can trigger rehash on cycle detection
// Set to false during rehash to prevent infinite recursion
static bool cuckoo_insert_internal(CuckooHashMap *map, int key, int value, 
                                    bool allow_rehash) {
    // First check if key already exists in either table
    size_t idx1 = h1(map, key);
    size_t idx2 = h2(map, key);
    
    // Check table1 for existing key
    if (map->table1[idx1].occupied && map->table1[idx1].key == key) {
        map->table1[idx1].value = value;  // Update value
        return true;
    }
    // Check table2 for existing key
    if (map->table2[idx2].occupied && map->table2[idx2].key == key) {
        map->table2[idx2].value = value;  // Update value
        return true;
    }
    
    // Key doesn't exist, need to insert
    // Start displacement chain
    int cur_key = key;
    int cur_value = value;
    int use_table1 = 1;  // Alternate between tables
    
    // Try up to MAX_DISPLACEMENTS before giving up
    for (int i = 0; i < MAX_DISPLACEMENTS; i++) {
        if (use_table1) {
            // Try to place in table1
            size_t idx = h1(map, cur_key);
            if (!map->table1[idx].occupied) {
                // Empty slot found 
                // Success!
                map->table1[idx].key = cur_key;
                map->table1[idx].value = cur_value;
                map->table1[idx].occupied = true;
                map->size++;
                return true;
            }
            // Slot occupied
            // Evict current resident
            int evicted_key = map->table1[idx].key;
            int evicted_value = map->table1[idx].value;
            // Place our key here
            map->table1[idx].key = cur_key;
            map->table1[idx].value = cur_value;
            // Now we need to relocate the evicted key
            cur_key = evicted_key;
            cur_value = evicted_value;
            use_table1 = 0;  // Try table2 next for evicted key
        } else {
            // Try to place in table2
            size_t idx = h2(map, cur_key);
            if (!map->table2[idx].occupied) {
                // Empty slot found
                // Success!
                map->table2[idx].key = cur_key;
                map->table2[idx].value = cur_value;
                map->table2[idx].occupied = true;
                map->size++;
                return true;
            }
            // Slot occupied
            // Evict current resident
            int evicted_key = map->table2[idx].key;
            int evicted_value = map->table2[idx].value;
            // Place our key here
            map->table2[idx].key = cur_key;
            map->table2[idx].value = cur_value;
            // Now we need to relocate the evicted key
            cur_key = evicted_key;
            cur_value = evicted_value;
            use_table1 = 1;  // Try table1 next for evicted key
        }
    }
    
    // Exceeded MAX_DISPLACEMENTS - likely a cycle
    // Need to rehash with new hash functions
    if (allow_rehash) {
        // Save the displaced key for reinsertion after rehash
        int displaced_key = cur_key;
        int displaced_value = cur_value;
        
        if (!cuckoo_rehash(map)) {
            return false;  // Rehash failed
        }
        
        // Try inserting the displaced key with new hash functions
        return cuckoo_insert_internal(map, displaced_key, displaced_value, true);
    }
    
    return false;  // Failed and can't rehash
}

// Rehash the tables with new hash functions
// Reinserts all existing entries
static bool cuckoo_rehash(CuckooHashMap *map) {
    // Save old tables
    CuckooEntry *old_table1 = map->table1;
    CuckooEntry *old_table2 = map->table2;
    size_t old_capacity = map->capacity;
    size_t old_size = map->size;
    
    // Allocate new tables
    // Keep same capacity for simplicity
    map->table1 = calloc(old_capacity, sizeof(CuckooEntry));
    map->table2 = calloc(old_capacity, sizeof(CuckooEntry));
    
    if (!map->table1 || !map->table2) {
        // Allocation failed
        // Restore old tables
        free(map->table1);
        free(map->table2);
        map->table1 = old_table1;
        map->table2 = old_table2;
        return false;
    }
    
    // Generate new hash function seeds
    init_seeds(map);
    map->size = 0;          // Reset size for reinsertion
    map->rehash_count++;    // Track rehash for analysis
    
    // Reinsert all entries from old tables
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_table1[i].occupied) {
            // Don't allow nested rehash during rehash
            if (!cuckoo_insert_internal(map, old_table1[i].key, 
                                        old_table1[i].value, false)) {
                // Rehash failed
                // Restore old state
                free(map->table1);
                free(map->table2);
                map->table1 = old_table1;
                map->table2 = old_table2;
                map->size = old_size;
                return false;
            }
        }
        if (old_table2[i].occupied) {
            if (!cuckoo_insert_internal(map, old_table2[i].key, 
                                        old_table2[i].value, false)) {
                // Rehash failed - restore old state
                free(map->table1);
                free(map->table2);
                map->table1 = old_table1;
                map->table2 = old_table2;
                map->size = old_size;
                return false;
            }
        }
    }
    
    // Success
    // Free old tables
    free(old_table1);
    free(old_table2);
    return true;
}

// Public insert function
bool cuckoo_put(CuckooHashMap *map, int key, int value) {
    if (!map) return false;
    
    /*
    * Check load factor and expand if too high
    * Code adapted from Kutzelnigg, R. (2006). Bipartite random graphs and cuckoo hashing.
    * Proceedings of the Fourth Colloquium on Mathematics and Computer Science, 400-406.
    * Retrieved from https://arxiv.org/abs/cs/0604034.
    */ 
    double load = (double)map->size / (2.0 * map->capacity);
    if (load > 0.45 && map->size > 0) {
        // Need to expand tables
        size_t new_capacity = map->capacity * 2;
        CuckooEntry *old_table1 = map->table1;
        CuckooEntry *old_table2 = map->table2;
        size_t old_capacity = map->capacity;
        
        // Allocate larger tables
        map->table1 = calloc(new_capacity, sizeof(CuckooEntry));
        map->table2 = calloc(new_capacity, sizeof(CuckooEntry));
        
        if (map->table1 && map->table2) { // Allocation succeeded
            // Update capacity and reinsert all existing entries
            size_t old_size = map->size;
            map->capacity = new_capacity;
            map->size = 0;
            init_seeds(map);  // New hash functions for new capacity
            
            // Reinsert all existing entries
            bool success = true;
            for (size_t i = 0; i < old_capacity && success; i++) {
                if (old_table1[i].occupied) {
                    success = cuckoo_insert_internal(map, old_table1[i].key,
                                                     old_table1[i].value, false);
                }
                if (old_table2[i].occupied && success) {
                    success = cuckoo_insert_internal(map, old_table2[i].key,
                                                     old_table2[i].value, false);
                }
            }
            
            if (success) {
                free(old_table1);
                free(old_table2);
            } else {
                // Expansion failed
                // Restore old tables
                free(map->table1);
                free(map->table2);
                map->table1 = old_table1;
                map->table2 = old_table2;
                map->capacity = old_capacity;
                map->size = old_size;
            }
        } else {
            // Allocation failed
            // Restore old tables
            free(map->table1);
            free(map->table2);
            map->table1 = old_table1;
            map->table2 = old_table2;
        }
    }
    
    // Insert the key
    // Return true if inserted, false if rehash needed and failed
    return cuckoo_insert_internal(map, key, value, true);
}

// Lookup: O(1) worst case
// Check exactly 2 locations
// Return true if found, false otherwise
bool cuckoo_get(CuckooHashMap *map, int key, int *value) {
    if (!map) return false;
    
    // Check position in table1
    size_t idx1 = h1(map, key);
    if (map->table1[idx1].occupied && map->table1[idx1].key == key) {
        if (value) *value = map->table1[idx1].value;
        return true;
    }
    
    // Check position in table2
    size_t idx2 = h2(map, key);
    if (map->table2[idx2].occupied && map->table2[idx2].key == key) {
        if (value) *value = map->table2[idx2].value;
        return true;
    }
    
    return false;  // Not in either location
}

// Delete: O(1) worst case
// Check exactly 2 locations
// Return true if deleted, false if not found
bool cuckoo_delete(CuckooHashMap *map, int key) {
    if (!map) return false;
    
    // Check table1
    size_t idx1 = h1(map, key);
    if (map->table1[idx1].occupied && map->table1[idx1].key == key) {
        map->table1[idx1].occupied = false;  // Mark as empty
        map->size--;
        return true;
    }
    
    // Check table2
    size_t idx2 = h2(map, key);
    if (map->table2[idx2].occupied && map->table2[idx2].key == key) {
        map->table2[idx2].occupied = false;  // Mark as empty
        map->size--;
        return true;
    }
    
    return false;  // Key not found
}

// Return total number of stored elements
size_t cuckoo_size(CuckooHashMap *map) {
    return map ? map->size : 0;
}

// Calculate total memory usage
size_t cuckoo_memory_usage(CuckooHashMap *map) {
    if (!map) return 0;
    // Main struct + both tables
    return sizeof(CuckooHashMap) + 2 * map->capacity * sizeof(CuckooEntry);
}

// Return number of rehashes
// Useful for performance analysis
int cuckoo_rehash_count(CuckooHashMap *map) {
    return map ? map->rehash_count : 0;
}

// Calculate current load factor
// Load = elements / (2 * capacity) since we have 2 tables
double cuckoo_load_factor(CuckooHashMap *map) {
    if (!map || map->capacity == 0) return 0.0;
    return (double)map->size / (2.0 * map->capacity);
}