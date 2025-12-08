[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/zBqi0PeJ)
# Research Paper
* Name: Siddharth Kakked
* Semester: Fall 2025
* Topic: Cuckoo Hashing: A Hash Table with Worst-Case Constant Lookup Time


## Introduction

Cuckoo hashing is a hash table that achieves $O(1)$ worst-case lookup by giving each key two possible addresses instead of one. Unlike traditional hash tables where collisions can create long chains, cuckoo hashing guarantees that any key can be found by checking at most two locations. The name is derived from brood parasitism, a behavior in which a cuckoo chick pushes other eggs or young out of the nest upon hatching. Analogously, inserting a new key into a cuckoo hash table may push an older key to a different location [1].A key is stored in one of its two possible locations, so lookup simply checks both. Unlike traditional hash tables where collisions can create long chains, cuckoo hashing guarantees that any key can be found by checking at most two locations.[1]

Cuckoo hashing was introduced by Rasmus Pagh and Flemming Friche Rodler in 2001 at the European Symposium on Algorithms, with the full paper published in the Journal of Algorithms in 2004 [1]. The authors developed this data structure to provide a practical dictionary with worst-case constant lookup time, matching the theoretical performance of dynamic perfect hashing [2] while being significantly simpler to implement. The original paper established key theoretical results including the requirement that load factor remain below approximately 50%, and that the expected number of displacements during insertion is constant [1]. The work was recognized with the ESA Test-of-Time Award in 2020, acknowledging its significant impact on both theoretical computer science and practical systems design[1]. Since its introduction, numerous variants have been developed, including d-ary cuckoo hashing which uses more than two hash functions to achieve higher load factors [3], and the recent "bubble-up" technique by Kuszmaul and Mitzenmacher which optimizes insertion performance [4].

This report analyzes cuckoo hashing its history and its theoretical foundations (time and space complexity), empirical benchmarks, real-world applications, and details of my implementation in C.


## Analysis of Datastructure

**Overview**

The cuckoo hashing data structure consists of two hash tables $T_1$ and $T_2$, each of size $m$, and two independent hash functions $h_1$ and $h_2$. Every key $k$ is stored in exactly two locations: $T_1[h_1(k)]$ or $T_2[h_2(k)]$[1].

At any given time, a stored key occupies one of these two positions. On insertion, if the target position is already occupied, cuckoo hashing evicts the existing key and moves it to its alternate position, possibly triggering a chain of evictions. If too many displacements occur, the table is rehashed with new hash functions. This design keeps lookups simple while still allowing high load factors[1].

**Time Complexity**

To analyze time complexity, let $n$ be the number of stored keys, and let $m$ be the capacity of each table so that the total number of slots is $2m$. The load factor is $\alpha = \frac{n}{2m}$.[1,5]

Under standard assumptions that $h_1$ and $h_2$ behave like independent random functions, the expected behavior is:

- **Lookup** [1]  
  To determine whether a key $k$ is present, cuckoo hashing checks at most two locations:
  
  $T_1[h_1(k)]$ and $T_2[h_2(k)]$
  
  This work is constant and does not depend on $n$.

  $T_{\text{lookup}}(n) = O(1)$ (worst case)


- **Deletion** [1] 
  Deletion uses the same locations as lookup. If the key is found in either candidate position, it is removed and the slot is marked empty.

  $T_{\text{delete}}(n) = O(1)$

- **Insertion** [1] 
  Insertion is more involved because of possible displacements. For a new key $k$, the algorithm:

  1. Tries to place it at $T_1[h_1(k)]$.
  2. If that slot is occupied, evicts the resident key and tries to insert that evicted key into its alternate table.
  3. Repeats this process until an empty slot is found or a maximum number of displacements is reached.

  Under typical load factors below a critical threshold $\alpha_c$, the **expected** number of displacements per insertion is constant, so

  $\mathbb{E}\bigl[T_{\text{insert}}(n)\bigr] = O(1)$

  In the **worst case**, however, the insertion can enter a cycle that prevents successful placement. When a cycle is detected (for example, after a fixed limit of displacement steps), the entire table is rebuilt with new hash functions. A rebuild touches all $n$ keys, so a single insertion can cost

  $T_{\text{insert}}(n) = O(n)$

  in the worst case. The key point is that such rebuilds are rare under the random hashing assumptions, so the amortized expected cost of insertion remains $O(1)$.

- **Summary**

| Operation | Expected time      | Worst case time |
|----------|--------------------|-----------------|
| Lookup   | $O(1)$             | $O(1)$          |
| Insert   | $O(1)$ amortized   | $O(n)$ (rehash) |
| Delete   | $O(1)$             | $O(1)$          |


**Space Complexity**

Cuckoo hashing maintains two tables $𝑇_1$ and $𝑇_2$ of equal size $𝑚$. Each entry typically stores a key (and possibly an associated value) and a small amount of metadata, such as a flag indicating whether the slot is occupied. The total number of slots across both tables is $2m$. For a target load factor $\alpha$, we choose $m$ so that

$n \approx 2m\alpha \quad \Rightarrow \quad m \approx \frac{n}{2\alpha}$

Since $\alpha$ is treated as a constant, this implies that space usage is $\text{Space}(n) = \Theta(m) = \Theta(n)$, which is linear in the number of stored keys. Cuckoo hashing does not require auxiliary pointer structures such as linked lists inside buckets, so there is relatively little overhead beyond the tables themselves[1].


**General analysis of the datastructure**

Cuckoo hashing keeps its core operations simple while using a displacement chain to resolve collisions. The table maintains two arrays $T_1$ and $T_2$ with hash functions $h_1$ and $h_2$, and each key $k$ is allowed to live in exactly one of the two positions $T_1[h_1(k)]$ or $T_2[h_2(k)]$. Lookup computes $h_1(k)$ and $h_2(k)$, inspects those two cells, and either returns the value or reports that the key is absent, so it always performs a constant number of probes and has worst case time $\Theta(1)$. Deletion mirrors lookup: the structure checks both candidate positions for $k$ and, if found, clears that slot, with no need for tombstones or list manipulation. Insertion is where the displacement chain appears. To insert a key $k$, the algorithm first tries to place it at $T_1[h_1(k)]$; if that slot is empty, the operation finishes immediately, but if it already holds some key $k'$, then $k$ takes that position and $k'$ is evicted and moved to its alternate location $T_2[h_2(k')]$. If that alternate slot is also occupied, the resident key is evicted in turn and moved back to its own alternate position in $T_1$, and this process repeats, producing a sequence of evictions that alternates between $T_1$ and $T_2$ until an empty slot is found. Under standard random hashing assumptions this displacement chain has expected length $O(1)$, so most insertions involve only a small number of moves. Occasionally, however, the chain forms a cycle in which moving one key eventually causes the algorithm to revisit a previous configuration of keys and positions; implementations detect this by imposing a maximum number of allowed displacements per insertion, and if that limit is exceeded the table performs a rehash, choosing new hash functions and reinserting all keys so that the invariant can be restored.[1,5,6]


## Empirical Analysis

To evaluate the cuckoo hashing I implemented three different hash tables in C that all store 32 bit integer keys and values. Cuckoo hashing (two tables, two hash functions, displacement and rehash) is compared against two simpler and widely used collision resolution strategies: separate chaining (one table with linked lists in each bucket) and linear probing (open addressing with tombstones). For each structure, the load factor was fixed to 50% by choosing the capacity to be roughly twice the number of elements, and then measure insertion, lookup, and deletion time as the table size grows.

**Experiment 1: Worst case analysis**

*Hypothesis.* As the number of stored elements $n$ increases at a fixed load factor, the empirical worst case lookup cost will grow for separate chaining and linear probing (longer chains and probe sequences), while cuckoo hashing will maintain a strict constant bound of two probes per lookup.

To test this hypothesis, a worst case experiment was conducted that measures the maximum work required by each implementation to answer successful lookups. For each size $n \in$ {1000, 5000, 10000, 50000, 100000} the three hash tables (chained hashing, linear probing, and cuckoo hashing) were constructed and populated with $n$ uniformly generated 32 bit integer keys at an approximate load factor of $50\%$ by choosing the capacity to be about $2n$. The benchmark then scanned all keys and recorded three metrics: the maximum chain length observed in any bucket of the chained table, the maximum number of probes required by the linear probing table to locate a key, and the maximum number of locations inspected by the cuckoo table. Table 1 summarizes these worst case statistics.

*Table 1. Empirical worst case lookup behavior at roughly 50\% load factor.*

| Elements ($n$) | Chained: max chain length | Linear probing: max probes | Cuckoo: max lookups |
|---------------:|---------------------------|----------------------------|----------------------|
|          1000  | 5                         | 13                         | 2                    |
|          5000  | 4                         | 17                         | 2                    |
|         10000  | 5                         | 20                         | 2                    |
|         50000  | 6                         | 38                         | 2                    |
|        100000  | 7                         | 29                         | 2                    |

Across the tested sizes, the worst case cost for separate chaining and linear probing increases with $n$, while cuckoo hashing remains bounded by a constant. The maximum chain length for chaining grows from 4–5 nodes at smaller sizes to 6–7 nodes at $n = 50000$ and $n = 100000$, and the longest probe sequence for linear probing reaches 38 probes before fluctuating around 29 due to randomness in the key distribution. In contrast, cuckoo hashing always completes lookups in at most two probes, exactly matching the theoretical $O(1)$ worst case guarantee. Although these worst case values are still small relative to $n$ at the tested load factor, the trend illustrates the qualitative difference: the worst case lookup cost of chaining and linear probing can grow with table size and clustering, while cuckoo hashing maintains a strict constant upper bound.

**Experiment 2: Scaling behavior of hashmap insertions**

*Hypothesis.* At a fixed load factor of approximately $50\%$, the total insertion time will grow approximately linearly with $n$ for all three hash tables, reflecting $O(1)$ expected cost per insertion, with linear probing exhibiting the smallest constant factor, separate chaining intermediate, and cuckoo hashing the largest due to displacement chains and occasional rehashing.

To evaluate how each hash table behaves as the problem size grows, a scaling experiment was conducted that measures the total insertion time as a function of the number of elements. For each size (1000, 5000, 10000, 25000, 50000, 75000, 100000), the three implementations (chained hashing, linear probing, and cuckoo hashing) were constructed and populated with $n$ uniformly generated 32 bit integer keys while maintaining a load factor of approximately $50\%$ by choosing the capacity to be about $2n$. Figure 1 plots insertion time in milliseconds against the number of elements on a logarithmic $x$ axis, with separate curves for each collision resolution strategy. This scaling test provides a direct visual comparison of how the constant factors and growth behavior of the three designs differ as tables move from small toy sizes to more realistic workloads.

*Figure 1. Scaling behavior of chained hashing, linear probing, and cuckoo hashing at roughly 50% load factor.*
![Scaling behavior of hash map insertions](https://github.com/CS5008Fall2025/research-paper-skakked/blob/main/graphs/scaling_plot.png)

Across all tested sizes, linear probing achieves the lowest insertion time, with chained hashing consistently slower and cuckoo hashing the most expensive, especially beyond about $𝑛=25000$. This reflects the extra work cuckoo hashing performs during insertions (displacements and occasional rehashes), even though it offers the strongest worst case guarantees for lookup.

**Experiment 3: Lookup Performance**

*Hypothesis.* At a fixed load factor of approximately $50\%$, the total time for $n$ successful lookups will grow very slowly with $n$ for all three hash tables, consistent with $O(1)$ average cost per lookup, and cuckoo hashing will achieve equal or lower lookup latency than separate chaining and linear probing because each query inspects at most two candidate locations.

To evaluate lookup performance under the same conditions as the insertion experiment, a second experiment was conducted that measures the total time required to perform successful lookups on tables of increasing size. For each size $n \in${1000, 5000, 10000, 25000, 50000, 75000, 100000}, the three implementations (chained hashing, linear probing, and cuckoo hashing) were first constructed and populated with $n$ uniformly generated 32 bit integer keys at an approximate load factor of $50%$ by setting the capacity to about $2n$. The benchmark then issued $n$ successful lookup queries using the same key set and recorded the total time spent in lookup operations. Figure 2 plots lookup time in milliseconds against the number of elements on a logarithmic $x$ axis, with separate curves for each collision resolution strategy.

*Figure 2. Lookup performance of chained hashing, linear probing, and cuckoo hashing at roughly 50% load factor.*
![Lookup Performance Plot](https://github.com/CS5008Fall2025/research-paper-skakked/blob/main/graphs/lookup_plot.png)


Across all tested sizes, lookup time grows very slowly with $n$, consistent with the expected $O(1)$ average behavior at a fixed load factor, but the relative ordering of the three schemes changes with scale. At small to medium sizes cuckoo hashing is at least competitive and often best; for example, at $n = 1000$ both chained and cuckoo complete all lookups in about $0.006$ ms compared with $0.008$ ms for linear probing, and at $n = 25000$ cuckoo takes $0.144$ ms versus $0.174$ ms for chained and $0.197$ ms for linear probing. At larger sizes, however, linear probing becomes the fastest: at $n = 100000$ and $50%$ load it achieves $1.529$ ms for $n$ successful lookups compared with $1.613$ ms for chained hashing and $1.846$ ms for cuckoo hashing, roughly a $17%$ improvement over cuckoo. These results show that all three designs deliver constant time lookups in practice at fixed load, with cuckoo hashing slightly favored at smaller scales and linear probing offering the best lookup throughput once tables become large in this implementation.


## Application

Cuckoo hashing is used in systems that require predictable, low latency hash table lookups rather than only good average case behavior. Its two position lookup and bounded probe pattern make it valuable across several domains.

### Network Packet Processing 

Cuckoo hashing is widely used in software routers and middleboxes to implement fast dictionaries for flow tables, NAT mappings, access control lists, and other per flow state [7]. Frameworks like Intel’s Data Plane Development Kit (DPDK) provide high performance cuckoo hash tables, and systems such as CuckooSwitch build Ethernet forwarding tables on top of DPDK’s hash library to process packets at line rate[8,9]. This is valuable because routers and switches must sustain millions of lookups per second, and a single slow lookup can cause packet drops or buffer buildup, so cuckoo hashing’s small, bounded number of probes helps keep latency stable and throughput high.[7-9]

### In Memory Caches and Key Value Stores

In memory caches and key value stores use cuckoo hashing for their index structures when workloads are read heavy and low latency is critical. MemC3, a Memcached variant from Carnegie Mellon, is a prominent example that adopts “optimistic cuckoo hashing” as its key value index and reports higher space occupancy and better throughput than stock Memcached. Reads only need to check two fixed locations, so many lookups can proceed without locks while writers handle occasional displacements, which improves concurrency, cache locality, and memory utilization.[10,11]

### Hardware and FPGA Implementations

Cuckoo hashing is also used in on chip key value stores and lookup tables implemented on FPGAs and ASICs. FPGA based key value stores use cuckoo hashing to achieve high memory utilization and pipeline friendly access patterns, and several open source designs implement cuckoo based key value engines in Verilog as research prototypes[12]. Hardware designers prefer cuckoo hashing because its fixed number of candidate locations per key maps naturally to parallel BRAM or SRAM lookups, avoiding pointer chasing and unbounded probe sequences and making timing closure easier in deep pipelines.[12-14].

### Database and Storage Indexing

Database and storage systems use cuckoo hashing for hash based index structures and file formats that prioritize consistent point lookup latency. RocksDB, for example, offers a CuckooTable SSTable format based on cuckoo hashing for workloads dominated by point lookups, providing one or two memory accesses per query, and research on “cuckoo index” structures uses cuckoo filters to create lightweight secondary indexes over large datasets [15]. These designs are attractive because storage systems often care about 95th or 99th percentile latency, and the bounded number of probes in cuckoo hashing helps keep index lookup time predictable enough to satisfy strict service level objectives [16].


## Implementation

### Language and Environment

The cuckoo hash map is implemented in C using the C99 standard and compiled with GCC using the flags `-Wall -Wextra -std=c99 -O2`. C was chosen to provide direct control over memory layout and allocation, which fits the low level nature of hash table design and performance benchmarking. The implementation does not depend on any third party libraries. It uses only the C standard library headers `stdlib.h`, `string.h`, `time.h`, and `stdbool.h` for memory management, string operations, timing, and Boolean types.

### Project Structure

The codebase is organized into modular components so that each collision resolution strategy can be tested behind a common interface:

```text
src/
├── cuckoo.h / cuckoo.c                 # Cuckoo hash map implementation
├── chained.h / chained.c               # Chained hash map (comparison)
├── linear_probing.h / linear_probing.c # Linear probing (comparison)
├── test_utils.h / test_utils.c         # Timing and key generation helpers
├── test_correctness.h / test_correctness.c   # Unit tests for all maps
├── test_benchmarks.h / test_benchmarks.c     # Performance benchmarks
└── test_main.c                         # Entry point with CLI parsing
```

The `test_main.c` file provides a simple command line interface that lets the user choose which implementation to run, which experiment to execute, and how many elements to insert. The shared test harness ensures that chained hashing, linear probing, and cuckoo hashing all see identical workloads and capacity choices.

### Core Data Structures

The cuckoo hash map maintains two separate tables that use independent hash functions:

```c
typedef struct {
    int key;
    int value;
    bool occupied;
} CuckooEntry;

typedef struct {
    CuckooEntry *table1;   // First hash table
    CuckooEntry *table2;   // Second hash table
    size_t capacity;       // Capacity of EACH table
    size_t size;           // Total elements across both tables
    unsigned int seed1;    // Seed for first hash function
    unsigned int seed2;    // Seed for second hash function
    int rehash_count;      // Number of rehashes performed
} CuckooHashMap;
```

Each `CuckooEntry` stores a single `(key, value)` pair along with an `occupied` flag that tracks whether the slot is in use. The map struct holds pointers to both tables, a shared capacity value (each table has the same length), the current element count, and seeds that parameterize the two hash functions. The `rehash_count` field records how many times the structure has been rebuilt due to insertion cycles.

### Key Implementation Details

#### Hash Function Design

Cuckoo hashing relies on having two independent hash functions so that each key has two candidate homes. The implementation uses a MurmurHash inspired mixing function parameterized by a seed:

```c
static size_t hash_with_seed(int key, unsigned int seed, size_t capacity) {
    unsigned int k = (unsigned int)key;
    k ^= seed;           // Mix in the seed for independence
    k ^= (k >> 16);      // Bit mixing
    k *= 0x85ebca6b;     // Magic constant from MurmurHash3
    k ^= (k >> 13);
    k *= 0xc2b2ae35;
    k ^= (k >> 16);
    return k % capacity;
}
```

Two hash wrappers `h1` and `h2` call this helper with `seed1` and `seed2` respectively. Seeds are initialized when the map is created and regenerated on every rehash so that problematic configurations get new hash functions. Using a proper mixing function avoids clustering, especially for sequential integer keys, and makes the empirical results more representative of textbook cuckoo hashing.

#### Insertion with Displacement Chain

Insertion is the most complex operation. It implements the characteristic cuckoo displacement process that alternates between the two tables:

```c
bool cuckoo_insert_internal(CuckooHashMap *map, int key, int value, bool count_size) {
    int cur_key = key;
    int cur_value = value;
    int use_table1 = 1;

    for (int i = 0; i < MAX_DISPLACEMENTS; i++) {
        if (use_table1) {
            size_t idx = h1(map, cur_key);
            if (!map->table1[idx].occupied) {
                // Empty slot found - success
                map->table1[idx].key = cur_key;
                map->table1[idx].value = cur_value;
                map->table1[idx].occupied = true;
                if (count_size) {
                    map->size++;
                }
                return true;
            }
            // Evict current resident
            int evicted_key = map->table1[idx].key;
            int evicted_value = map->table1[idx].value;
            map->table1[idx].key = cur_key;
            map->table1[idx].value = cur_value;
            cur_key = evicted_key;
            cur_value = evicted_value;
            use_table1 = 0;  // Try table2 next
        } else {
            size_t idx = h2(map, cur_key);
            if (!map->table2[idx].occupied) {
                // Empty slot found - success
                map->table2[idx].key = cur_key;
                map->table2[idx].value = cur_value;
                map->table2[idx].occupied = true;
                if (count_size) {
                    map->size++;
                }
                return true;
            }
            // Evict current resident
            int evicted_key = map->table2[idx].key;
            int evicted_value = map->table2[idx].value;
            map->table2[idx].key = cur_key;
            map->table2[idx].value = cur_value;
            cur_key = evicted_key;
            cur_value = evicted_value;
            use_table1 = 1;  // Bounce back to table1
        }
    }

    // If control reaches here, we probably have a cycle
    return false;
}
```

On each step the algorithm chooses a table, computes the corresponding index, and either places the key into an empty slot or evicts the existing resident and continues with that evicted key. The `MAX_DISPLACEMENTS` constant (set to 500) bounds the length of the displacement chain. If this limit is reached the insertion is treated as failed and a rehash is triggered.

Lookup and deletion are straightforward in comparison. Both operations compute `h1` and `h2` for the key and inspect at most two positions. Deletion simply clears the `occupied` flag for any matching entry and decrements `size`.

#### Rehashing Strategy

When a cycle is detected the map regenerates its hash function seeds and reinserts all elements into freshly allocated tables. The rehash logic looks like this:

```c
static bool cuckoo_rehash(CuckooHashMap *map) {
    size_t old_capacity = map->capacity;
    CuckooEntry *old_table1 = map->table1;
    CuckooEntry *old_table2 = map->table2;

    // Allocate fresh tables with the same capacity
    map->table1 = calloc(old_capacity, sizeof(CuckooEntry));
    map->table2 = calloc(old_capacity, sizeof(CuckooEntry));
    if (!map->table1 || !map->table2) {
        // Allocation failed, restore old tables and signal error
        free(map->table1);
        free(map->table2);
        map->table1 = old_table1;
        map->table2 = old_table2;
        return false;
    }

    // Generate new hash function seeds
    init_seeds(map);
    map->size = 0;
    map->rehash_count++;

    // Reinsert all entries with new hash functions
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_table1[i].occupied) {
            cuckoo_insert_internal(map,
                                   old_table1[i].key,
                                   old_table1[i].value,
                                   true);
        }
        if (old_table2[i].occupied) {
            cuckoo_insert_internal(map,
                                   old_table2[i].key,
                                   old_table2[i].value,
                                   true);
        }
    }

    free(old_table1);
    free(old_table2);
    return true;
}
```

Rehashing keeps the same capacity in this implementation because the experiments already control load factor separately. In a production setting the rehash could also grow the table when the load factor becomes too high.

### Implementation Challenges

Several aspects of the implementation required careful attention:

* **Managing two tables and hash seeds:** Keeping `table1`, `table2`, `seed1`, and `seed2` in sync across insertions, deletions, and rehashes is easy to get wrong. Small mistakes in index calculations or seed initialization tend to show up as hard to debug missing keys.
* **Choosing `MAX_DISPLACEMENTS`:** There is a trade off between allowing enough displacements to resolve most insertions without rehashing and avoiding long cycles that hurt latency. The value 500 worked well in practice for the tested sizes and load factors.
* **Memory management:** Rehashing allocates new tables and must correctly free the old ones after all entries have been moved. The code needs to handle allocation failures gracefully and avoid memory leaks when rehashing or destroying the map.
* **Fair benchmarking:** To compare chained hashing, linear probing, and cuckoo hashing fairly, the test harness had to ensure that all implementations used the same hash function family, capacities, and workloads. Encapsulating timing and key generation in `test_utils.c` helped keep these details consistent across experiments.


## Summary

The analysis of cuckoo hashing demonstrated that its theoretical guarantees translate clearly into measurable practical behavior. The structure’s defining property is that there are two possible homes per key. It proved effective in ensuring strictly bounded lookup time, which remained constant across all tested table sizes. In contrast, both chained hashing and linear probing showed increasing worst case behavior as the number of elements grew, with longer chains and probe sequences emerging even at moderate load factors. The empirical experiments confirmed that all three hash table designs deliver close to linear scaling for insertions and lookups under a fixed load factor, but they differ significantly in constant factors. Linear probing emerged as the fastest in terms of throughput due to its strong cache locality, while cuckoo hashing incurred higher insertion costs because of displacement chains and occasional rehashing. Despite this overhead, cuckoo hashing maintained competitive lookup performance and was the only structure that provided true worst case \(O(1)\) lookup behavior.

Through building the implementation from scratch in C, the project highlighted complexity that underlies cuckoo hashing. Managing displacement chains, avoiding infinite loops, designing robust hash functions, and implementing rehashing safely required far more care than simpler strategies like chaining or linear probing. Overall, the project strengthened understanding of how algorithmic guarantees interact with hardware behavior, memory layout, and real world workloads, and it demonstrated why cuckoo hashing continues to influence modern system design.


## References 
[1] Pagh, R. and Rodler, F. F. 2004. Cuckoo hashing. Journal of Algorithms 51, 2 (2004), 122–144. DOI:https://doi.org/10.1016/j.jalgor.2003.12.002

[2] Martin Dietzfelbinger, Anna Karlin, Kurt Mehlhorn, Friedhelm Meyer auf der Heide, Hans Rohnert, and Robert Endre Tarjan. 1994. Dynamic perfect hashing – upper and lower bounds. SIAM Journal on Computing 23, 4 (1994), 738–761. DOI:10.1137/S0097539791194094.

[3] Dimitris Fotakis, Rasmus Pagh, Peter Sanders, and Paul Spirakis. 2005. Space efficient hash tables with worst case constant access time. Theory of Computing Systems 38, 2 (2005), 229–248. DOI:10.1007/s00224-004-1195-x

[4] William Kuszmaul and Michael Mitzenmacher. 2025. Efficient d-ary cuckoo hashing at high load factors by bubbling up. In Proceedings of the 2025 ACM–SIAM Symposium on Discrete Algorithms (SODA 2025). Society for Industrial and Applied Mathematics.

[5] A. Kirsch, M. D. Mitzenmacher, and U. Wieder. 2010. More robust hashing: Cuckoo hashing with a stash. SIAM Journal on Computing 39, 4 (2010), 1543–1561. DOI:https://doi.org/10.1137/080728743.

[6] M. Aumüller, M. Dietzfelbinger, and P. Woelfel. 2014. Explicit and efficient hash families suffice for cuckoo hashing with a stash. Algorithmica 70, 3 (2014), 428–456. DOI:https://doi.org/10.1007/s00453-013-9840-x.

[7] N. Le Scouarnec. 2018. Cuckoo++ hash tables: High-performance hash tables for networking applications. In Proceedings of the ACM/IEEE Symposium on Architectures for Networking and Communications Systems (ANCS '18), 41–54. DOI:https://doi.org/10.1145/3230718.3232629

[8] Intel Corporation. 2024. Data Plane Development Kit (DPDK) Programmer's Guide: Hash Library. https://doc.dpdk.org/guides/prog_guide/hash_lib.html

[9]  D. Zhou, B. Fan, H. Lim, D. G. Andersen, and M. Kaminsky. 2013. Scalable, high performance Ethernet forwarding with CuckooSwitch. In Proceedings of the 9th ACM International Conference on Emerging Networking Experiments and Technologies (CoNEXT '13), 97–108. DOI:https://doi.org/10.1145/2535372.2535379

[10]  B. Fan, D. G. Andersen, and M. Kaminsky. 2013. MemC3: Compact and concurrent MemCache with dumber caching and smarter hashing. In Proceedings of the 10th USENIX Symposium on Networked Systems Design and Implementation (NSDI '13), 371–384.

[11] M. T. Goodrich, D. S. Hirschberg, M. Mitzenmacher, and J. Thaler. 2011. Fully de-amortized cuckoo hashing for cache-oblivious dictionaries and multimaps. arXiv preprint arXiv:1107.4378 (2011).

[12] Ruizhi Zhang, Sasindu Wijeratne, Yang Yang, Sanmukh R. Kuppannagari, and Viktor K. Prasanna. 2020. A High Throughput Parallel Hash Table on FPGA using XOR-based Memory. In 2020 IEEE High Performance Extreme Computing Conference (HPEC). IEEE, 1–7. https://doi.org/10.1109/HPEC43674.2020.9286199

[13] Mikhail Asiatici and Paolo Ienne. 2022. Request, Coalesce, Serve, and Forget: Miss-Optimized Memory Systems for Bandwidth-Bound Cache-Unfriendly Applications on FPGAs. ACM Transactions on Reconfigurable Technology and Systems 15, 2, Article 13 (June 2022), 33 pages. https://doi.org/10.1145/3466823Retry

[14] Zsolt István. 2013. Hash Table for Large Key-Value Stores on FPGAs. Master's thesis. ETH Zurich, Department of Computer Science, Systems Group, Zurich, Switzerland. https://doi.org/10.3929/ethz-a-007636118

[15] Yuanyuan Sun, Yu Hua, Dan Feng, Ling Yang, Pengfei Zuo, Shunde Cao, and Yuncheng Guo. 2017. A Collision-Mitigation Cuckoo Hashing Scheme for Large-Scale Storage Systems. IEEE Transactions on Parallel and Distributed Systems 28, 3 (March 2017), 619–632. https://doi.org/10.1109/TPDS.2016.2591011

[16] Andreas Kipf, Damian Chromejko, Alexander Hall, Peter Boncz, and David G. Andersen. 2020. Cuckoo Index: A Lightweight Secondary Index Structure. Proceedings of the VLDB Endowment 13, 13 (September 2020), 3559–3572. https://doi.org/10.14778/3424573.3424577

[17]

[18]