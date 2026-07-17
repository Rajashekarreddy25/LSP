# Linux System Programming Handbook

# Chapter 9
# Atomic Operations & Memory Ordering (Masterclass)

---

# Table of Contents

1. Why Atomic Operations Exist
2. What is Atomicity?
3. Why counter++ is NOT Atomic
4. CPU Instruction Breakdown
5. Race Condition Revisited
6. What is an Atomic Operation?
7. Hardware Support
8. Compare-And-Swap (CAS)
9. Load Linked / Store Conditional (LL/SC)
10. GCC Atomic Built-ins
11. C11 Atomic Library
12. Memory Ordering
13. Memory Barriers
14. How Mutexes Use Atomic Operations
15. Lock-Free Programming
16. Common Mistakes
17. Interview Questions
18. Summary

---

# 1. Why Atomic Operations Exist?

Let's start with a simple question.

Suppose

```c
counter++;
```

Question

How many CPU instructions execute?

Many beginners answer

```
One
```

Wrong.

It is usually multiple instructions.

That is why race conditions occur.

Atomic operations exist to make certain operations appear

```
Indivisible
```

No thread can observe the operation halfway through.

---

# 2. What Does "Atomic" Mean?

Atomic means

```
Cannot Be Interrupted
```

Imagine pressing a button.

Either

```
Completed
```

or

```
Not Started
```

Never

```
Half Completed
```

Atomic operations provide exactly this guarantee.

---

# 3. Why counter++ is NOT Atomic

Suppose

```c
counter++;
```

The compiler may generate operations conceptually like

```
Load counter

↓

Add 1

↓

Store counter
```

Example

Initially

```
counter = 10
```

CPU

```
Load 10

↓

Add 1

↓

Store 11
```

Three separate steps.

---

# 4. Race Condition Revisited

Suppose

```
counter = 100
```

Thread1

```
Load 100
```

Scheduler switches.

Thread2

```
Load 100

↓

Add

↓

Store 101
```

Scheduler switches.

Thread1

```
Add

↓

Store 101
```

Expected

```
102
```

Actual

```
101
```

One increment disappeared.

---

# 5. Visual Timeline

```
Time →

Thread1

Load 100

--------------------

Thread2

          Load 100

          Add

          Store 101

--------------------

Thread1

Add

Store 101
```

Lost update.

---

# 6. Atomic Increment

Instead of

```
Load

↓

Add

↓

Store
```

Hardware performs

```
Increment

As One Atomic Action
```

Another CPU

cannot observe

halfway.

---

# 7. Hardware Support

Modern CPUs provide special instructions.

Examples

```
LOCK XADD

CMPXCHG

XCHG

Atomic Exchange

Atomic Increment
```

(Exact instructions vary by architecture.)

These instructions are guaranteed to execute atomically with respect to other processors for the specified memory location.

---

# 8. Compare-And-Swap (CAS)

CAS is one of the most important atomic primitives.

Many synchronization algorithms are built using it.

Prototype conceptually

```
CAS(

address,

expected,

new_value

)
```

Meaning

```
IF

memory == expected

THEN

write new_value

ELSE

do nothing
```

---

Example

Memory

```
100
```

Thread wants

```
100 → 101
```

CAS checks

```
Is value still 100?
```

Yes

```
Store 101

Success
```

Suppose another thread already changed it.

Memory

```
102
```

CAS

```
Expected 100

Actual 102

Fail
```

Thread retries.

---

# 9. CAS Timeline

Initial

```
counter = 50
```

Thread1

```
CAS

50 → 51

Success
```

Thread2

```
CAS

50 → 51

Fails

Current Value = 51
```

Retry

```
CAS

51 → 52

Success
```

No update is lost.

---

# 10. Why CAS is Powerful

Using CAS,

we can build

```
Mutex

Spinlock

Semaphore

Lock-Free Queue

Lock-Free Stack
```

without using a traditional lock internally.

---

# 11. Load Linked / Store Conditional (LL/SC)

Some CPU architectures use LL/SC instead of CAS.

Conceptually:

```
Load Linked

↓

Remember Address

↓

Modify Value

↓

Store Conditional
```

If another CPU modified the value in the meantime,

the store fails.

The thread retries.

---

# 12. GCC Atomic Built-ins

Older GCC provides

```c
__sync_fetch_and_add()
```

Example

```c
int old =
__sync_fetch_and_add(
    &counter,
    1);
```

Meaning

```
Return Old Value

↓

Increment Counter
```

---

Modern GCC recommends the `__atomic_*` family.

Example

```c
__atomic_fetch_add(
    &counter,
    1,
    __ATOMIC_SEQ_CST);
```

The last argument specifies the desired memory ordering.

---

# 13. C11 Atomic Library

Header

```c
#include <stdatomic.h>
```

Variable

```c
atomic_int counter = 0;
```

Increment

```c
atomic_fetch_add(
        &counter,
        1);
```

Now

```
Thread Safe
```

No mutex required for this simple operation.

---

# 14. Why Not Always Use Atomics?

Because

not every operation

is simple.

Suppose

```
Read

↓

Modify

↓

Update

↓

Write File

↓

Update Queue

↓

Signal Thread
```

This is a complex transaction.

Atomic variables cannot replace a mutex for protecting multiple related operations.

---

# 15. Memory Ordering

This is where things become interesting.

Suppose

Thread1

```c
data = 100;

ready = 1;
```

Thread2

```c
while(ready == 0);

printf("%d",data);
```

Looks correct.

But modern CPUs and compilers may reorder independent memory operations unless synchronization prevents it.

Without appropriate synchronization,

another thread may observe updates in an unexpected order.

This is why memory ordering matters.

---

# 16. Compiler Reordering

Compiler sees

```
Instruction A

Instruction B
```

It may reorder them

if the language rules allow,

to improve performance,

provided the observable behavior of a single-threaded program is unchanged.

In multithreaded programs,

atomic operations and synchronization primitives define what reorderings are allowed.

---

# 17. CPU Reordering

Modern CPUs also execute instructions

```
Out Of Order
```

to improve performance.

Multiple instructions may be in flight simultaneously.

Without synchronization,

another CPU may observe memory operations in a different order.

---

# 18. Memory Barrier

A memory barrier (also called a memory fence)

prevents certain types of reordering.

Conceptually

```
Write A

↓

Memory Barrier

↓

Write B
```

Other threads observing the synchronization are guaranteed to observe A before B according to the chosen memory ordering.

---

# 19. Types of Memory Ordering (C11)

The C11 atomic library supports several memory orders.

### memory_order_relaxed

Only atomicity is guaranteed.

No ordering with respect to other operations.

Fastest.

---

### memory_order_acquire

Used when reading synchronization state.

Prevents later reads/writes from moving before the acquire.

---

### memory_order_release

Used when publishing data.

Ensures earlier writes become visible before the release operation.

---

### memory_order_acq_rel

Combines acquire and release semantics.

---

### memory_order_seq_cst

Sequential consistency.

Simplest to reason about.

Often the easiest choice until performance tuning is necessary.

---

# 20. How Mutexes Use Atomic Operations

People often ask

```
How

does

pthread_mutex_lock()

work?
```

Conceptually

```
Atomic Compare-And-Swap

↓

If Lock Free

Acquire

↓

Else

Sleep
```

So,

mutexes themselves rely on atomic hardware operations to safely change lock state.

---

# 21. Lock-Free Programming

Traditional

```
Mutex

↓

Sleep

↓

Wake
```

Lock-Free

```
Atomic

↓

Retry

↓

Continue
```

No thread owns a lock.

Threads coordinate using atomic primitives.

Lock-free algorithms can improve scalability,

but they are significantly harder to design correctly.

---

# 22. Common Mistakes

Mistake

Thinking

```
volatile

=

Atomic
```

Wrong.

`volatile` prevents certain compiler optimizations.

It does **not** make operations atomic and does **not** synchronize threads.

---

Mistake

Replacing every mutex

with atomics.

Wrong.

Atomics are best for small shared states.

Complex shared data structures still often require locks.

---

Mistake

Ignoring memory ordering.

Using relaxed ordering everywhere without understanding visibility requirements can introduce subtle bugs.

---

# 23. Interview Questions

Q.

Why is counter++ not atomic?

Answer

Because it typically consists of multiple operations (load, modify, store), allowing interference from other threads.

---

Q.

What is CAS?

Answer

Compare-And-Swap.

It atomically compares a memory location with an expected value and, if they match, writes a new value.

---

Q.

Difference between volatile and atomic?

Answer

`volatile`

- Prevents certain compiler optimizations.
- Does not provide synchronization.

`atomic`

- Provides atomic operations.
- Supports well-defined synchronization and memory ordering.

---

Q.

Do mutexes internally use atomic operations?

Answer

Yes.

Modern mutex implementations typically use atomic instructions for the fast path and operating system support (such as Linux futexes) when blocking is required.

---

Q.

When should atomics be used?

Answer

For simple shared variables such as counters, flags, reference counts, and lock-free algorithms.

---

# Summary

| Feature | Atomic Operation |
|----------|------------------|
| Interrupted Halfway | No |
| Lock Needed | Not Always |
| Thread Safe | Yes (for the atomic operation itself) |
| CPU Support | Yes |
| Used In | Mutexes, Spinlocks, Lock-Free Structures |
| Complex Transactions | Usually Need Locks |

---

# Key Points to Remember

✓ Atomic means indivisible.

✓ `counter++` is usually **not** atomic.

✓ CAS is one of the fundamental building blocks of modern synchronization.

✓ `volatile` is **not** a synchronization mechanism.

✓ Memory ordering is just as important as atomicity.

✓ Many synchronization primitives are built on top of atomic CPU instructions.

---

End of Chapter 9
