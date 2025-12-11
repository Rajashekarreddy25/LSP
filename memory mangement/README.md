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



 11. How is fragmentation managed in memory allocation?

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

######  12. Describe the concept of paging.

Paging divides memory into fixed-size blocks:

- Virtual memory → pages

- Physical memory → frames

Each virtual page is mapped to a physical frame via page tables.

✔️ Eliminates external fragmentation
✔️ Allows virtual memory
✔️ Allows processes to use more memory than RAM (using swap)

#####  13. Explain segmentation.

Segmentation divides memory into variable-sized logical segments, such as:

- Code segment

- Data segment

- Stack segment

Each segment has:

- Base address

- Limit (size)

Segmentation reflects program structure.

#####  14. Difference between paging and segmentation

| **Paging**                        | **Segmentation**                          |
| --------------------------------- | ----------------------------------------- |
| Fixed-size pages                  | Variable-size segments                    |
| Eliminates external fragmentation | Causes external fragmentation             |
| Physical memory managed in frames | Logical memory based on program structure |
| Address → page number + offset    | Address → segment number + offset         |
| Used heavily in Linux             | Not used much; Linux mainly uses paging   |

#####  15. Define page table.

A page table is a data structure maintained by the OS that maps:

####### Virtual page number → Physical frame number

Every process has its own page table in Linux.

#####  16. Define Memory Management Unit (MMU).

The MMU is hardware that handles:

- Address translation (virtual → physical)

- Memory protection

- TLB lookups

- Paging support

Located in the CPU.

#####  17. Explain the role of MMU in memory management.

MMU performs:

✔️ Virtual-to-physical address translation

Using page tables.

✔️ Memory protection

Prevents illegal access → segmentation faults.

✔️ TLB caching

Speeds up translation.

✔️ Supporting paging and virtual memory

#####  18. Describe the Translation Lookaside Buffer (TLB).

TLB is a cache inside MMU storing recently used page table entries.

Purpose:

 - Reduce time taken for address translation

 - Avoid frequent page table lookups

#####  19. What is a TLB miss? How is it handled?
- TLB Miss = Requested virtual page not found in TLB
How it is handled:

1. MMU looks into the page table

2. Loads the correct entry into the TLB

3. Restarts the memory access

4. If the page is not in RAM → page fault occurs

#####  20. Discuss the working principle of MMU.

MMU works in this sequence:

1. CPU generates a virtual address

2. MMU checks TLB for mapping

3. If present → sends physical address to RAM

3. If not present → walks through page tables

4. Inserts entry into TLB

5. If page not in RAM → triggers page fault

6. Kernel loads page from disk → updates page table

7. MMU retries access

#####  21. Explain the concept of address translation in MMU.

- Address translation = converting a virtual address into a physical address.

Steps:

1. Virtual address split into

- Page number

- Offset

2. Page number → physical frame using page table

3.Physical address = frame number + offset

######  22. How does MMU support virtual memory?

MMU enables virtual memory by:

- Mapping virtual pages to physical frames

- Allowing pages to be swapped in/out to disk

- Protecting per-process address space

- Supporting page tables and TLB

This gives each process a huge memory space, even if RAM is small.

#####  23. Describe the process of page table traversal in MMU.

Page table traversal steps:

1. TLB miss occurs

2. MMU reads page directory (top-level)

3. Finds page table for that range

4. Reads page table entry

5. Gets physical frame number

6. Builds physical address

7. Loads entry into TLB

#####  24. What is page fault handling in MMU?

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

#####  25. Explain the page replacement algorithms used in MMU.

When RAM is full, MMU + OS must select a page to remove.

Common algorithms:

- FIFO

- Optimal

- LRU

- Clock / Second-chance

These decide which page to swap out.

#####  26. Define page replacement algorithms.

- A page replacement algorithm chooses which memory page to remove from RAM when a new page must be loaded, but RAM is full.

Goal:

- Minimize page faults

- Improve performance

#####  27. Describe FIFO page replacement algorithm.

FIFO = First-In-First-Out
The page loaded earliest is removed first.

Steps:

1. Maintain a queue of loaded pages

2. Replace the oldest page

3. Simple, but not optimal.

#####  28. Discuss the optimal page replacement algorithm.

- Optimal algorithm replaces the page that will not be used for the longest time in the future.

- Best page-fault performance, but impossible to implement in real systems (future is unknown).

- Used for theoretical comparison.

#####  29. Explain the LRU (Least Recently Used) page replacement algorithm.

LRU removes the page that has not been used for the longest time.

Assumption:
Recently used pages are more likely to be used again.

LRU uses:

- Counters

 - Stacks

- Hardware support (in real MMUs)

It performs close to optimal.


######  30. What is the Clock Page Replacement Algorithm?

Clock Algorithm is an improved version of FIFO.
It uses a circular buffer (like a clock) of pages with a use/ reference bit for each page.

Steps:

1. A pointer (clock hand) moves over pages.

2. For each page it checks reference bit (R):

- R = 1 → Recently used → Set R = 0 and skip

- R = 0 → Replace this page

3. Continue rotating until a replaceable page is found.

✔️ Faster and better than FIFO.

#####  31. Advantages and Disadvantages of Page Replacement Algorithms
FIFO

✔️ Easy to implement
✖️ Removes oldest page → may remove important pages
✖️ Belady’s anomaly

Optimal

✔️ Lowest page faults
✖️ Requires future knowledge → not practical

LRU

✔️ Close to optimal
✖️ Needs hardware counters or stack → costly

Clock

✔️ Efficient and simple
✔️ Good performance
✖️ Still approximate, not perfect

######  32. Compare and contrast different page replacement algorithms

| Algorithm     | Method                      | Performance | Practicality      |
| ------------- | --------------------------- | ----------- | ----------------- |
| FIFO          | Replace oldest              | Poor        | High (simple)     |
| Optimal       | Replace farthest future use | Best        | Not implementable |
| LRU           | Replace least recently used | Very good   | Medium cost       |
| Clock         | Approximates LRU            | Good        | Highly practical  |
| Second Chance | FIFO + reference bit        | Good        | Practical         |

#####  33. NRU (Not Recently Used) Page Replacement Algorithm

Pages are classified into 4 classes using Reference bit (R) and Modified bit (M):

| Class | R | M | Meaning                |
| ----- | - | - | ---------------------- |
| 0     | 0 | 0 | Not used, not modified |
| 1     | 0 | 1 | Modified but not used  |
| 2     | 1 | 0 | Used but clean         |
| 3     | 1 | 1 | Used and modified      |


Algorithm:

1. Prefer to replace lowest class (0).

2. Higher class → less chance of replacement.

#####  34. Second Chance Page Replacement Algorithm

Improved FIFO.

Steps:

1. Select oldest page

2. If R = 0 → Replace it

3. If R = 1 → Set R = 0 and move it to end of queue

This gives frequently used pages a “second chance”.

######  35. Enhancements to Basic Page Replacement Algorithms

1. Use + Dirty bit

2. Clock-Pro (used in Linux)

3. Adaptive replacement (ARC)

4. Working-set based algorithms

5. Two-handed clock algorithm

Linux uses advanced variants combining LRU + Clock.


#####  36. Define segmentation in memory management.

Segmentation divides memory into logical units called segments:

- Code

- Data

- Stack

- Heap

Each segment has a base and limit.

#####  37. Benefits of segmentation

✔️ Matches program structure
✔️ Supports sharing (code segment shareable)
✔️ Easy protection per segment
✔️ Allows variable-sized memory regions

#####  38. Disadvantages of segmentation

✖️ Causes external fragmentation
✖️ Complex memory allocation
✖️ Harder to manage in large systems
✖️ Variable-size allocation overhead

#####  39. Describe the implementation of segmentation.

Segment table stores:

- Segment base address

- Segment limit

- Access permissions

Virtual address = segment number + offset

CPU uses segment registers to translate to physical memory.   

#####  40. Segmentation fault and its causes

- A segmentation fault (SIGSEGV) occurs when process accesses invalid memory.

Causes:

- Accessing NULL pointers

- Buffer overflow

- Writing into read-only segment

- Accessing memory outside allocated range

- Stack overflow

Linux sends SIGSEGV to terminate program.

#####  41. Explain the concept of segment registers.

Registers that store:

- Base address of segment

- Size limit

- Access rights

Examples in x86:

- CS (Code Segment)

- DS (Data Segment)

- SS (Stack Segment)

#####  42. What is a segment table?

A table containing entries for each segment:

- Base address

- Limit

- Protection bits

Used by CPU + MMU for address translation.

#####  43. How does segmentation support protection and sharing?

- Each segment has access rights (read/write/execute) → protection

- Segments like code can be shared between processes → sharing

- Illegal access causes segmentation fault
  
#####  44. Discuss segmentation with paging approach.

Used in many systems (e.g., x86 protected mode).

Steps:

1. Program divided into segments

2. Each segment divided into pages

3. Pages mapped to physical frames

This removes fragmentation problems.

#####  45. Compare segmentation vs paging

| Feature       | Segmentation          | Paging               |
| ------------- | --------------------- | -------------------- |
| Size          | Variable              | Fixed                |
| Fragmentation | External              | No external          |
| Logical view  | Logical units         | Physical units       |
| Protection    | Per segment           | Per page             |
| Useful for    | Human view of program | Hardware memory mgmt |

#####  46. Define memory fragmentation.

Condition where free memory is split into small blocks, making it unusable.

#####  47. Explain the causes of memory fragmentation.

- Variable-size allocations

- Frequent malloc/free operations

- Long-running processes

- Segmentation (variable blocks)

- Different memory lifetimes

#####  48. How does memory fragmentation affect system performance?

- More page faults

- Slower allocations

-Low RAM utilization

- Kernel may invoke OOM killer

- Poor cache locality

#####  49. Techniques to reduce fragmentation

1. Paging

2. Memory pools

3. Slab allocator (Linux kernel)

4. Binning by size (tcmalloc, jemalloc)

5. Compaction

6. Best-fit / Buddy Allocator

##### 50. Explain compaction as a technique for reducing fragmentation.

Compaction moves allocated blocks together to create one large free region.

Example:
```
A    B      C   D
↓ compact
A B C D
free space: large block
```
#####  52. Working of memory compaction algorithms

Steps:

1. Identify scattered free blocks

2. Move allocated blocks towards one end

3. Update pointers/addresses

4. Create a continuous free region

Used in:

- Garbage-collected languages

- Kernel buddy allocator

##### 53. Challenges in implementing memory compaction

- Moving memory is expensive

- Needs address updating

- Requires CPU time

- Risk of data corruption

- Complex in real-time systems

Linux uses buddy allocator + compaction carefully.

##### 54. Explain memory fragmentation in embedded systems.

Embedded systems face fragmentation due to:

- Limited RAM

- No virtual memory

- Long uptime

- Dynamic memory usage

Fragmentation can cause:

- System crashes

- Allocation failures

- CPU stalls

#####  55. How does memory allocation impact fragmentation?

- Small frequent allocations → more fragmentation

- Variable size blocks → higher fragmentation

- Poor allocator (first-fit) → more fragmentation

- Using pools → reduces fragmentation


#####  56. Define memory mapping.

Mapping between:

- Virtual memory → Physical memory

- File → Virtual memory

- Device → Memory region

Used for:

- Executables

- Shared memory

- Memory-mapped IO

#####  57. Purpose of memory mapping

- Speed up file access

- Reduce RAM usage

- Enable shared memory between processes

- Enable device access in embedded/OS

Linux uses mmap() for mapping.

#####  58. Memory mapping techniques

1. File mapping (mmap a file)

2. Anonymous mapping (RAM mapping)

3. Shared memory mapping

4. Device memory mapping

5. Kernel memory mapping

#####  59. What is memory-mapped I/O?

Memory-mapped I/O maps device registers into the memory address space.

CPU reads/writes device as if reading/writing a memory location.

Used in:

- Embedded systems

- GPU

- Network cards

- UART

- Flash controllers

Example in Linux: accessing /dev/mem or mmap() on device drivers.

##### 60. Explain memory-mapped files.

Memory-mapped files allow a file on disk to be mapped directly into virtual memory, so the file contents can be accessed like an array in RAM.

In Linux:
```
void *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
```

After mapping:

- Reading memory → reads from file

- Writing memory → writes to file

- Processes do not use read() / write(); they simply access memory.

#####  61. Advantages of memory mapping

-  Faster file access
-  Reduced system calls → less CPU overhead
-  Easy to share memory between processes
-  Automatic caching by OS
-  Better for large files
-  Simplifies coding (just pointer access)

#####  62. Drawbacks of memory mapping

- ✖ Cannot map extremely huge files on small RAM systems
- ✖ Page faults occur frequently at first access
- ✖ Errors propagate as segmentation faults
- ✖ Harder to handle random crashes
- ✖ Not suitable for real-time embedded systems

#####  63. How memory mapping improves performance

-  Eliminates read() / write() calls
-  No data copying between kernel/user space
- Uses page cache efficiently
- Only needed portions of file are loaded into memory (demand paging)
- OS automatically handles replacement using page replacement algorithms


##### 64. Explain memory-mapped graphics.

Graphics hardware exposes a framebuffer (video memory).
By memory-mapping this framebuffer, programs draw pixels directly by writing memory.

Example:
```
uint32_t *fb = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);
fb[x + y*width] = 0xFF0000;  // draw a red pixel
```

Used in:

Linux /dev/fb0

Embedded displays

GPU programming

#####  65. Memory mapping in embedded systems

Used to map:

Sensors

- Flash memory

- I/O registers

- DMA buffers

- LCD framebuffers

Important because:

- Embedded systems have limited RAM

- Faster hardware access

- Efficient device control (UART, SPI)

##### 66. Define cache memory.

- Cache memory is a small, fast memory inside the CPU that stores frequently accessed data.

##### 67. Purpose of cache memory

- Reduce CPU access time
- Bridge speed gap between CPU and RAM
- Improve program performance
- Reduce memory latency

##### 68. Types of cache memory
1. L1 cache

- Smallest & fastest

- Inside CPU core

- Split into L1-I (instruction) & L1-D (data)

2. L2 cache

- Larger, slower than L1

- Per-core or shared

3. L3 cache

- Large & slower

- Shared among CPU cores

##### 69. Cache Coherence Problem

Occurs in multi-core CPUs when each core has its own cache.

If one core updates a memory location, others must see the updated value.

Solutions:

- MESI protocol

- MOESI protocol

- Snooping

- Directory-based coherence

##### 70. Cache Replacement Policies

Used when cache is full.

Common algorithms:

- LRU (Least Recently Used)

- FIFO

- Random

- Clock (for caches too)
#####  71. What is cache associativity?

Associativity defines how many places a block can go inside cache.

Types:

- Direct-mapped (1 place)

- 2-way set associative (2 places)

- Fully associative (anywhere)

Higher associativity → lower conflict misses.

##### 72. Working of cache memory

Steps:

1. CPU requests memory address

2. Cache checks if data exists

  - If yes → cache hit

  - If no → cache miss

3. On miss → load block from RAM into cache

4. Return data to CPU

#####  73. Cache hit and cache miss

- Cache hit → Data found in cache

- Cache miss → Data not found; must fetch from RAM

Miss types:

- Cold miss

- Conflict miss

- Capacity miss

##### 74. Importance of cache memory in memory management

- Reduces page faults
- Speeds up address translation
- Improves TLB performance
- Faster context switching
- Better CPU pipeline utilization

##### 75. Cache memory in memory hierarchy

Cache sits between CPU and RAM:
```
CPU → L1 → L2 → L3 → RAM → Disk
```

Fastest memories are closest to CPU.
