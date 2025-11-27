 /**
 * Linear Probing Hash Map Implementation
 * Name: Siddharth Kakked
 * Semester: Fall 2025
 * Class: CS 5008
 */

#include "linear_probing.h"
#include <stdlib.h>
#include <string.h>

/* Simple hash function */
static size_t hash(int key, size_t capacity) { // Jenkins' One-at-a-time hash
    unsigned int h = (unsigned int)key; // Ensure non-negative
    h ^= (h >> 16); 
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);
    return h % capacity; // Modulo capacity
}

// Create a new linear probing hash map with given capacity
LinearHashMap* linear_create(size_t capacity) {
    if (capacity == 0) capacity = 16;       /* Default capacity */
    
    LinearHashMap *map = malloc(sizeof(LinearHashMap));  /* Allocate struct */
    if (!map) return NULL;
    
    map->entries = calloc(capacity, sizeof(LinearEntry)); /* Zeroed array */
    if (!map->entries) {
        free(map);
        return NULL;
    }
    
    /* calloc sets all bytes to 0, so state = EMPTY (which is 0) */
    map->capacity = capacity;
    map->size = 0;
    return map;
}

void linear_destroy(LinearHashMap *map) {
    if (!map) return;
    free(map->entries);                     /* Free array */
    free(map);                              /* Free struct */
}

// Insert or update a key-value pair
bool linear_put(LinearHashMap *map, int key, int value) {
    if (!map) return false;
    
    /* Reject if load factor > 70% */
    if (map->size >= map->capacity * 7 / 10) {
        return false;                       /* Would need resize */
    }
    
    size_t idx = hash(key, map->capacity);  /* Starting position */
    size_t start = idx;                     /* Remember start */
    
    size_t first_deleted = map->capacity;   /* Sentinel: no deleted found */
    
    do {
        /* Case 1 : Empty Slot
        if (map->entries[idx].state == EMPTY) {
            if (first_deleted < map->capacity) {
                idx = first_deleted;        /* Prefer deleted slot */
            }
            map->entries[idx].key = key;
            map->entries[idx].value = value;
            map->entries[idx].state = OCCUPIED;
            map->size++;
            return true;
        }
        
        /* Case 2: Deleted slot - remember for later */
        if (map->entries[idx].state == DELETED && first_deleted == map->capacity) {
            first_deleted = idx;            /* Save first deleted position */
        }
        
        /* Case 3: Occupied with same key */
        if (map->entries[idx].state == OCCUPIED && map->entries[idx].key == key) {
            map->entries[idx].value = value; /* Update existing */
            return true;
        }
        
        idx = (idx + 1) % map->capacity;    /* Linear probe: next slot */
    } while (idx != start);                 /* Full circle? */
    
    /* Table full but found a deleted slot */
    if (first_deleted < map->capacity) {
        map->entries[first_deleted].key = key;
        map->entries[first_deleted].value = value;
        map->entries[first_deleted].state = OCCUPIED;
        map->size++;
        return true;
    }
    
    return false;                           /* Table completely full */
}

bool linear_get(LinearHashMap *map, int key, int *value) {
    if (!map) return false;
    
    size_t idx = hash(key, map->capacity);  /* Starting position */
    size_t start = idx;
    
    do {
        /* Empty slot = key doesn't exist */
        if (map->entries[idx].state == EMPTY) {
            return false;                   /* Not found */
        }
        
        /* Found the key */
        if (map->entries[idx].state == OCCUPIED && map->entries[idx].key == key) {
            if (value) *value = map->entries[idx].value;
            return true;                    /* Success */
        }
        
        /* DELETED slots: keep probing (key might be past it) */
        idx = (idx + 1) % map->capacity;    /* Next slot */
    } while (idx != start);                 /* Full circle? */
    
    return false;                           /* Not found */
}

// Delete a key. Returns true if key existed.
bool linear_delete(LinearHashMap *map, int key) {
    if (!map) return false;
    
    size_t idx = hash(key, map->capacity);  /* Starting position */
    size_t start = idx;
    
    do {
        /* Empty slot = key doesn't exist */
        if (map->entries[idx].state == EMPTY) {
            return false;                   /* Not found */
        }
        
        /* Found the key */
        if (map->entries[idx].state == OCCUPIED && map->entries[idx].key == key) {
            map->entries[idx].state = DELETED;  /* Mark as deleted (tombstone) */
            map->size--;
            return true;
        }
        
        idx = (idx + 1) % map->capacity;    /* Next slot */
    } while (idx != start);
    
    return false;                           /* Not found */
}

size_t linear_size(LinearHashMap *map) {
    return map ? map->size : 0;
}

size_t linear_memory_usage(LinearHashMap *map) {
    if (!map) return 0;
    
    size_t mem = sizeof(LinearHashMap);             /* Struct itself */
    mem += map->capacity * sizeof(LinearEntry);     /* Fixed-size array */
    
    return mem;                             /* Memory doesn't grow with size */
}

// Count probes needed to find or miss a key
int linear_probe_count(LinearHashMap *map, int key) {
    if (!map) return -1;
    
    size_t idx = hash(key, map->capacity);  /* Starting position */
    size_t start = idx;
    int probes = 0;
    
    do {
        probes++;                           /* Count this probe */
        
        /* Empty = end of probe sequence */
        if (map->entries[idx].state == EMPTY) {
            return probes;                  /* Probes until miss */
        }
        
        /* Found the key */
        if (map->entries[idx].state == OCCUPIED && map->entries[idx].key == key) {
            return probes;                  /* Probes until hit */
        }
        
        idx = (idx + 1) % map->capacity;
    } while (idx != start);
    
    return probes;                          /* Full table scan */
}