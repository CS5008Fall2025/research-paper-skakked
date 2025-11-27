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

