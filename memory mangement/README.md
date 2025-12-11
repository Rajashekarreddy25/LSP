##### 1. What is memory management in system programming?

Memory management is the process of controlling, allocating, and organizing the computer’s main memory.
It ensures that each program gets the memory it needs and prevents conflicts, while using memory efficiently.

##### 2. Define virtual memory.
Virtual memory is a memory management technique that gives an illusion of a large main memory by using a combination of RAM + disk space.

It allows programs to run even if the RAM is not enough.

##### 3. Differentiate between physical memory and virtual memory.
| **Physical Memory (RAM)**                  | **Virtual Memory**                   |
| ------------------------------------------ | ------------------------------------ |
| Actual hardware memory installed on system | Memory created using RAM + hard disk |
| Faster                                     | Slower (because disk access is slow) |
| Limited in size                            | Can be much larger than RAM          |
| Directly accessed by CPU                   | Accessed via OS and MMU              |
| No swapping                                | Supports swapping/paging             |

##### 4. What is the role of an operating system in memory management?

The OS manages memory by:

- Allocating memory to processes

- Deallocating memory when processes finish

- Preventing memory conflicts between processes

- Using paging/swapping to extend memory (virtual memory)

- Keeping track of which memory is free or used

- Ensuring protection and isolation between processes

##### 5. Explain the purpose of memory allocation.

Memory allocation means giving a block of memory to a program when it requests it.
Purpose:

- To store program instructions and data

- To ensure each program gets the memory it needs

- To make efficient use of available memory

##### 6. Describe the significance of memory deallocation.

Memory deallocation means releasing memory back to the system when a program is done using it.
Significance:

- Prevents memory leaks

- Makes memory available for future processes

- Improves overall performance and stability

##### 7. Define fragmentation in memory management.

- Fragmentation occurs when memory is divided into small, unusable pieces, making it hard to allocate continuous memory even if total free memory is enough.

##### 8. What are the types of fragmentation?

Two main types:

Internal Fragmentation

External Fragmentation

 ##### 9. Explain internal fragmentation.

- Internal fragmentation occurs when allocated memory is larger than the required memory.
- The unused part inside the allocated block is wasted.

##### Example:
- If a process needs 18 KB but OS allocates a 20 KB block → 2 KB wasted.

##### 10. Explain external fragmentation.

- External fragmentation occurs when free memory is broken into small scattered holes, and no continuous block is available for a process.

##### Example:
Total free memory = 100 KB, but scattered as 10 KB + 20 KB + 30 KB + 40 KB →
A process needing 50 KB cannot be allocated even though total free memory is enough.



✅ 11. How is fragmentation managed in memory allocation?

Linux handles fragmentation using:

1. Paging

Physical memory is divided into fixed-size pages → eliminates external fragmentation.

2. Slab Allocator

Used by kernel. Groups objects of same size → reduces internal fragmentation.

3. mmap() for large allocations

Big chunks allocated via virtual memory mapping → avoids heap fragmentation.

4. Compaction (in some systems)

Moves memory blocks to create a large continuous free block.
(Not common in Linux user space, but used internally in the kernel.)

###### ✅ 12. Describe the concept of paging.

Paging divides memory into fixed-size blocks:

- Virtual memory → pages

- Physical memory → frames

Each virtual page is mapped to a physical frame via page tables.

✔️ Eliminates external fragmentation
✔️ Allows virtual memory
✔️ Allows processes to use more memory than RAM (using swap)

##### ✅ 13. Explain segmentation.

Segmentation divides memory into variable-sized logical segments, such as:

- Code segment

- Data segment

- Stack segment

Each segment has:

- Base address

- Limit (size)

Segmentation reflects program structure.

##### ✅ 14. Difference between paging and segmentation

| **Paging**                        | **Segmentation**                          |
| --------------------------------- | ----------------------------------------- |
| Fixed-size pages                  | Variable-size segments                    |
| Eliminates external fragmentation | Causes external fragmentation             |
| Physical memory managed in frames | Logical memory based on program structure |
| Address → page number + offset    | Address → segment number + offset         |
| Used heavily in Linux             | Not used much; Linux mainly uses paging   |

##### ✅ 15. Define page table.

A page table is a data structure maintained by the OS that maps:

####### Virtual page number → Physical frame number

Every process has its own page table in Linux.

##### ✅ 16. Define Memory Management Unit (MMU).

The MMU is hardware that handles:

- Address translation (virtual → physical)

- Memory protection

- TLB lookups

- Paging support

Located in the CPU.

##### ✅ 17. Explain the role of MMU in memory management.

MMU performs:

✔️ Virtual-to-physical address translation

Using page tables.

✔️ Memory protection

Prevents illegal access → segmentation faults.

✔️ TLB caching

Speeds up translation.

✔️ Supporting paging and virtual memory

##### ✅ 18. Describe the Translation Lookaside Buffer (TLB).

TLB is a cache inside MMU storing recently used page table entries.

Purpose:

 - Reduce time taken for address translation

 - Avoid frequent page table lookups

##### ✅ 19. What is a TLB miss? How is it handled?
- TLB Miss = Requested virtual page not found in TLB
How it is handled:

1. MMU looks into the page table

2. Loads the correct entry into the TLB

3. Restarts the memory access

4. If the page is not in RAM → page fault occurs

##### ✅ 20. Discuss the working principle of MMU.

MMU works in this sequence:

1. CPU generates a virtual address

2. MMU checks TLB for mapping

3. If present → sends physical address to RAM

3. If not present → walks through page tables

4. Inserts entry into TLB

5. If page not in RAM → triggers page fault

6. Kernel loads page from disk → updates page table

7. MMU retries access

##### ✅ 21. Explain the concept of address translation in MMU.

- Address translation = converting a virtual address into a physical address.

Steps:

1. Virtual address split into

- Page number

- Offset

2. Page number → physical frame using page table

3.Physical address = frame number + offset

###### ✅ 22. How does MMU support virtual memory?

MMU enables virtual memory by:

- Mapping virtual pages to physical frames

- Allowing pages to be swapped in/out to disk

- Protecting per-process address space

- Supporting page tables and TLB

This gives each process a huge memory space, even if RAM is small.

##### ✅ 23. Describe the process of page table traversal in MMU.

Page table traversal steps:

1. TLB miss occurs

2. MMU reads page directory (top-level)

3. Finds page table for that range

4. Reads page table entry

5. Gets physical frame number

6. Builds physical address

7. Loads entry into TLB

##### ✅ 24. What is page fault handling in MMU?

Page fault occurs when a virtual page is:

- Not in RAM

- Invalid

- Accessed with wrong permissions

Page Fault Handling Steps:

1. MMU raises page fault exception

2. Kernel checks:

   - Is page valid?

   - Is it swapped out?

3. If swapped: load from disk → RAM

4. Update page table

5. Resume process

- If invalid → segmentation fault (SIGSEGV).

##### ✅ 25. Explain the page replacement algorithms used in MMU.

When RAM is full, MMU + OS must select a page to remove.

Common algorithms:

- FIFO

- Optimal

- LRU

- Clock / Second-chance

These decide which page to swap out.

##### ✅ 26. Define page replacement algorithms.

- A page replacement algorithm chooses which memory page to remove from RAM when a new page must be loaded, but RAM is full.

Goal:

- Minimize page faults

- Improve performance

##### ✅ 27. Describe FIFO page replacement algorithm.

FIFO = First-In-First-Out
The page loaded earliest is removed first.

Steps:

1. Maintain a queue of loaded pages

2. Replace the oldest page

3. Simple, but not optimal.

##### ✅ 28. Discuss the optimal page replacement algorithm.

--Optimal algorithm replaces the page that will not be used for the longest time in the future.

- Best page-fault performance, but impossible to implement in real systems (future is unknown).

- Used for theoretical comparison.

##### ✅ 29. Explain the LRU (Least Recently Used) page replacement algorithm.

LRU removes the page that has not been used for the longest time.

Assumption:
Recently used pages are more likely to be used again.

LRU uses:

- Counters

 - Stacks

- Hardware support (in real MMUs)

It performs close to optimal.
