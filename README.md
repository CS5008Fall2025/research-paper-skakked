[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/zBqi0PeJ)
# Research Paper
* Name: Siddharth Kakked
* Semester: Fall 2025
* Topic: Cuckoo Hashing: A Hash Table with Worst-Case Constant Lookup Time

Note the following is an example outline to help you. Please rework as you need, you do not need to follow the section heads and *YOU SHOULD NOT* make everything a bulleted list. This needs to read as an executive report/research paper. 

## Introduction

Cuckoo hashing is a hash table that generates $O(1)$ worst-case lookup by giving each key two possible homes instead of one Unline traditional hash tables where collsions can create long chains or probe sequences, cuckoo hashing gurantees that any key can be found checking at most two locations. The name derives from the behavior of some species of cuckoo, where the cuckoo chick pushes the other eggs or young out of the nest when it hatches in a variation of the behavior referred to as brood parasitism; analogously, inserting a new key into a cuckoo hashing table may push an older key to a different location in the table[1].

Cuckoo hashing was introduced by Rasmus Pagh and Flemming Friche Rodler in 2001 at the European Symposium on Algorithms, with the full paper published in the Journal of Algorithms in 2004 [1]. The authors developed this scheme to provide a practical dictionary with worst-case constant lookup time, matching the theoretical performance of dynamic perfect hashing [2] while being significantly simpler to implement. The original paper established key theoretical results including the requirement that load factor remain below approximately 50%, and that the expected number of displacements during insertion is constant. The work was recognized with the ESA Test-of-Time Award in 2020, acknowledging its significant impact on both theoretical computer science and practical systems design. Since its introduction, numerous variants have been developed, including d-ary cuckoo hashing which uses more than two hash functions to achieve higher load factors [3], and the recent "bubble-up" technique by Kuszmaul and Mitzenmacher which optimizes insertion performance [4].

Traditional hash tables such as chaining and linear probing provide $O(1)$ average-case lookup time, but their worst-case performance can degrade to $O(n)$ when many keys collide. This unpredictability is problematic for real-time systems that cannot tolerate occasional slow operations, network routers that must process packets at line rate, and any application requiring bounded response times. Cuckoo hashing solves this problem by guaranteeing that lookup always takes constant time, regardless of the input distribution or collision patterns.

This report provides a comprehensive analysis of cuckoo hashing. The following section presents the theoretical analysis including time complexity, space complexity, and a proof of correctness. The empirical analysis section describes experimental methodology and benchmark results demonstrating the algorithm's practical performance. The application section discusses real-world use cases where cuckoo hashing excels. The implementation section details the C implementation including design decisions and challenges encountered. Finally, the summary reflects on key findings and lessons learned.


## Analysis of Algorithm/Datastructure

**Overview**

The cuckoo hashing data structure consists of two hash tables $T_1$ and $T_2$, each of size $m$, and two independent hash functions $h_1$ and $h_2$. Every key $k$ is stored in exactly two locations: $T_1[h_1(k)]$ or $T_2[h_2(k)]$

At any given time, a stored key occupies one of these two positions. On insertion, if the target position is already occupied, cuckoo hashing evicts the existing key and moves it to its alternate position, possibly triggering a chain of evictions. If too many displacements occur, the table is rehashed with new hash functions. This design keeps lookups simple while still allowing high load factors.

**Time Complexity**

To analyze time complexity, let $n$ be the number of stored keys, and let $m$ be the capacity of each table so that the total number of slots is $2m$. The load factor is $\alpha = \frac{n}{2m}$.

Under standard assumptions that $h_1$ and $h_2$ behave like independent random functions, the expected behavior is:

- **Lookup**  
  To determine whether a key $k$ is present, cuckoo hashing checks at most two locations:
  
  $T_1[h_1(k)]$ and $T_2[h_2(k)]$
  
  This work is constant and does not depend on $n$.

  $T_{\text{lookup}}(n) = O(1)$ (worst case)


- **Deletion**  
  Deletion uses the same locations as lookup. If the key is found in either candidate position, it is removed and the slot is marked empty.

  $T_{\text{delete}}(n) = O(1)$

- **Insertion**  
  Insertion is more involved because of possible displacements. For a new key $k$, the algorithm:

  1. Tries to place it at $T_1[h_1(k)]$.
  2. If that slot is occupied, evicts the resident key and tries to insert that evicted key into its alternate table.
  3. Repeats this process until an empty slot is found or a maximum number of displacements is reached.

  Under typical load factors below a critical threshold $\alpha_c$, the **expected** number of displacements per insertion is constant, so

  $\mathbb{E}\bigl[T_{\text{insert}}(n)\bigr] = O(1)$

  In the **worst case**, however, the insertion can enter a cycle that prevents successful placement. When a cycle is detected (for example, after a fixed limit of displacement steps), the entire table is rebuilt with new hash functions. A rebuild touches all $n$ keys, so a single insertion can cost

  $T_{\text{insert}}(n) = O(n)$

  in the worst case. The key point is that such rebuilds are rare under the random hashing assumptions, so the amortized expected cost of insertion remains $O(1)$.

  **In Summary**

| Operation | Expected time      | Worst case time |
|----------|--------------------|-----------------|
| Lookup   | $O(1)$             | $O(1)$          |
| Insert   | $O(1)$ amortized   | $O(n)$ (rehash) |
| Delete   | $O(1)$             | $O(1)$          |


**Space Complexity**

Cuckoo hashing maintains two tables $𝑇_1$ and $𝑇_2$ of equal size $𝑚$. Each entry typically stores a key (and possibly an associated value) and a small amount of metadata, such as a flag indicating whether the slot is occupied. The total number of slots across both tables is $2m$. For a target load factor $\alpha$, we choose $m$ so that

$n \approx 2m\alpha \quad \Rightarrow \quad m \approx \frac{n}{2\alpha}$

Since $\alpha$ is treated as a constant, this implies that space usage is $\text{Space}(n) = \Theta(m) = \Theta(n)$, which is linear in the number of stored keys. Cuckoo hashing does not require auxiliary pointer structures such as linked lists inside buckets, so there is relatively little overhead beyond the tables themselves.

**General analysis of the algorithm/datastructure**

## Empirical Analysis
- What is the empirical analysis?
- Provide specific examples / data.


## Application
- What is the algorithm/datastructure used for?
- Provide specific examples
- Why is it useful / used in that field area?
- Make sure to provide sources for your information.


## Implementation
- What language did you use?
- What libraries did you use?
- What were the challenges you faced?
- Provide key points of the algorithm/datastructure implementation, discuss the code.
- If you found code in another language, and then implemented in your own language that is fine - but make sure to document that.


## Summary
- Provide a summary of your findings
- What did you learn?
