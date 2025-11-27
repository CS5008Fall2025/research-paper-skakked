/**
 * Cuckoo Hashing Implementation
 * 
 * Reference: Pagh, R., & Rodler, F. F. (2001). Cuckoo Hashing. 
 *            BRICS Report Series RS-01-32.
 */

#include "cuckoo_hash.h"
#include <stdio.h>
#include <math.h>

// Helpers and constants
static size_t next_power_of_2(size_t n) {
    n--;                        /* Handle exact powers of 2 */
    n |= n >> 1;                /* Smear bits right */
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;               /* Result is next power of 2 */
}

static uint32_t random_seed(void) {
    return (uint32_t)rand() ^ ((uint32_t)rand() << 16);  /* Combine two rand() calls */
}

static size_t calc_max_loop(size_t capacity) {
    if (capacity < 2) return 1;
    size_t log_val = 0;
    size_t temp = capacity;
    while (temp > 1) {          /* Calculate log2(capacity) */
        temp >>= 1;
        log_val++;
    }
    return MAX_LOOP_FACTOR * log_val;  /* MaxLoop = 3 * log(n) per paper */
}

// Hash functions
CuckooHash* cuckoo_create(size_t initial_capacity) {
    CuckooHash *ht = malloc(sizeof(CuckooHash));  /* Allocate struct */
    if (!ht) return NULL;
    
    size_t cap = next_power_of_2(initial_capacity);  /* Round up capacity */
    if (cap < 16) cap = 16;                          /* Minimum size 16 */
    
    ht->T1 = calloc(cap, sizeof(Entry));  /* Allocate table 1 (zeroed) */
    ht->T2 = calloc(cap, sizeof(Entry));  /* Allocate table 2 (zeroed) */
    
    if (!ht->T1 || !ht->T2) {             /* Check allocation success */
        free(ht->T1);
        free(ht->T2);
        free(ht);
        return NULL;
    }
    
    ht->capacity = cap;
    ht->size = 0;
    ht->seed1 = random_seed();            /* Random seed for h1 */
    ht->seed2 = random_seed();            /* Random seed for h2 */
    ht->max_loop = calc_max_loop(cap);    /* Set displacement limit */
    
    while (ht->seed2 == ht->seed1) {      /* Ensure different seeds */
        ht->seed2 = random_seed();
    }
    
    return ht;
}

void cuckoo_destroy(CuckooHash *ht) {
    if (ht) {
        free(ht->T1);   /* Free table 1 */
        free(ht->T2);   /* Free table 2 */
        free(ht);       /* Free struct */
    }
}

// Hash functions

bool cuckoo_lookup(CuckooHash *ht, uint32_t key, uint32_t *value) {
    if (!ht || key == EMPTY) return false;  /* Validate input */
    
    uint32_t pos1 = h1(ht, key);            /* Compute first hash */
    if (ht->T1[pos1].key == key) {          /* Check T1[h1(key)] */
        if (value) *value = ht->T1[pos1].value;
        return true;                         /* Found in T1 */
    }
    
    uint32_t pos2 = h2(ht, key);            /* Compute second hash */
    if (ht->T2[pos2].key == key) {          /* Check T2[h2(key)] */
        if (value) *value = ht->T2[pos2].value;
        return true;                         /* Found in T2 */
    }
    
    return false;                            /* Key not in table */
}


// Insert with cuckoo displacement

bool cuckoo_insert(CuckooHash *ht, uint32_t key, uint32_t value) {
    if (!ht || key == EMPTY) return false;  /* Validate input */
    
    /* --- Check if key exists (update if so) --- */
    uint32_t pos1 = h1(ht, key);
    if (ht->T1[pos1].key == key) {          /* Key in T1? */
        ht->T1[pos1].value = value;         /* Update value */
        return true;
    }
    uint32_t pos2 = h2(ht, key);
    if (ht->T2[pos2].key == key) {          /* Key in T2? */
        ht->T2[pos2].value = value;         /* Update value */
        return true;
    }
    
    /* --- Check load factor --- */
    double load = (double)ht->size / (2.0 * ht->capacity);
    if (load >= LOAD_THRESHOLD) {           /* Too full? */
        if (!cuckoo_resize(ht, ht->capacity * 2)) {
            return false;                    /* Resize failed */
        }
    }
    
    /* --- Cuckoo insertion with displacement --- */
    Entry current = {key, value};           /* Entry to insert */
    
    for (size_t i = 0; i < ht->max_loop; i++) {
        /* Try placing in T1 */
        pos1 = h1(ht, current.key);
        if (ht->T1[pos1].key == EMPTY) {    /* T1 slot empty? */
            ht->T1[pos1] = current;         /* Place here */
            ht->size++;
            return true;                     /* Success! */
        }
        
        Entry temp = ht->T1[pos1];          /* Save existing entry */
        ht->T1[pos1] = current;             /* Kick it out (cuckoo!) */
        current = temp;                      /* Now insert evicted key */
        
        /* Try placing in T2 */
        pos2 = h2(ht, current.key);
        if (ht->T2[pos2].key == EMPTY) {    /* T2 slot empty? */
            ht->T2[pos2] = current;         /* Place here */
            ht->size++;
            return true;                     /* Success! */
        }
        
        temp = ht->T2[pos2];                /* Save existing entry */
        ht->T2[pos2] = current;             /* Kick it out */
        current = temp;                      /* Now insert evicted key */
    }
    
    /* --- Cycle detected - rehash with new functions --- */
    if (!cuckoo_rehash(ht)) {
        return false;                        /* Rehash failed */
    }
    
    return cuckoo_insert(ht, current.key, current.value);  /* Retry */
}

// Delete key

bool cuckoo_delete(CuckooHash *ht, uint32_t key) {
    if (!ht || key == EMPTY) return false;  /* Validate input */
    
    uint32_t pos1 = h1(ht, key);
    if (ht->T1[pos1].key == key) {          /* Found in T1? */
        ht->T1[pos1].key = EMPTY;           /* Clear key */
        ht->T1[pos1].value = 0;             /* Clear value */
        ht->size--;
        return true;
    }
    
    uint32_t pos2 = h2(ht, key);
    if (ht->T2[pos2].key == key) {          // Found in T2? */
        ht->T2[pos2].key = EMPTY;           // Clear key */
        ht->T2[pos2].value = 0;             // Clear value */
        ht->size--;
        return true;
    }
    
    return false;                            // Key not found 
}

// Statistics

double cuckoo_load_factor(CuckooHash *ht) {
    if (!ht || ht->capacity == 0) return 0.0;
    return (double)ht->size / (2.0 * ht->capacity);  // n / (2m) 
}

size_t cuckoo_size(CuckooHash *ht) {
    return ht ? ht->size : 0;
}

size_t cuckoo_memory_usage(CuckooHash *ht) {
    if (!ht) return 0;
    
    size_t mem = sizeof(CuckooHash);                // Struct size */
    mem += 2 * ht->capacity * sizeof(Entry);        // T1 + T2 arrays */
    
    return mem;
}

/* ============================================================
 * Rehash - new hash functions, same capacity
 * ============================================================ */

bool cuckoo_rehash(CuckooHash *ht) {
    if (!ht) return false;
    
    /* Collect all existing entries */
    Entry *entries = malloc(ht->size * sizeof(Entry));
    if (!entries) return false;
    
    size_t count = 0;
    for (size_t i = 0; i < ht->capacity && count < ht->size; i++) {
        if (ht->T1[i].key != EMPTY) {       /* Entry in T1? */
            entries[count++] = ht->T1[i];   /* Save it */
        }
        if (ht->T2[i].key != EMPTY) {       /* Entry in T2? */
            entries[count++] = ht->T2[i];   /* Save it */
        }
    }
    
    /* Clear both tables */
    memset(ht->T1, 0, ht->capacity * sizeof(Entry));
    memset(ht->T2, 0, ht->capacity * sizeof(Entry));
    ht->size = 0;
    
    /* Generate new hash function seeds */
    ht->seed1 = random_seed();
    ht->seed2 = random_seed();
    while (ht->seed2 == ht->seed1) {        /* Ensure different */
        ht->seed2 = random_seed();
    }
    
    /* Reinsert all entries with new hash functions */
    bool success = true;
    for (size_t i = 0; i < count; i++) {
        if (!cuckoo_insert(ht, entries[i].key, entries[i].value)) {
            success = false;
            break;
        }
    }
    
    free(entries);
    return success;
}

/* ============================================================
 * Resize - grow/shrink tables
 * ============================================================ */

bool cuckoo_resize(CuckooHash *ht, size_t new_capacity) {
    if (!ht) return false;
    
    new_capacity = next_power_of_2(new_capacity);   /* Round up */
    if (new_capacity < 16) new_capacity = 16;       /* Minimum size */
    
    /* Collect existing entries */
    Entry *entries = malloc(ht->size * sizeof(Entry));
    if (!entries) return false;
    
    size_t count = 0;
    for (size_t i = 0; i < ht->capacity && count < ht->size; i++) {
        if (ht->T1[i].key != EMPTY) {
            entries[count++] = ht->T1[i];
        }
        if (ht->T2[i].key != EMPTY) {
            entries[count++] = ht->T2[i];
        }
    }
    
    /* Allocate new larger tables */
    Entry *new_T1 = calloc(new_capacity, sizeof(Entry));
    Entry *new_T2 = calloc(new_capacity, sizeof(Entry));
    
    if (!new_T1 || !new_T2) {               /* Allocation failed? */
        free(new_T1);
        free(new_T2);
        free(entries);
        return false;
    }
    
    /* Replace old tables with new */
    free(ht->T1);
    free(ht->T2);
    ht->T1 = new_T1;
    ht->T2 = new_T2;
    ht->capacity = new_capacity;
    ht->size = 0;
    ht->max_loop = calc_max_loop(new_capacity);
    
    /* New hash function seeds */
    ht->seed1 = random_seed();
    ht->seed2 = random_seed();
    while (ht->seed2 == ht->seed1) {
        ht->seed2 = random_seed();
    }
    
    /* Reinsert all entries */
    bool success = true;
    for (size_t i = 0; i < count; i++) {
        if (!cuckoo_insert(ht, entries[i].key, entries[i].value)) {
            success = false;
            break;
        }
    }
    
    free(entries);
    return success;
}