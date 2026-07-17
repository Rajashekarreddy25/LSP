# Linux System Programming Handbook

# Chapter 3
# Semaphore

---

# Table of Contents

1. Introduction
2. Why Do We Need Semaphores?
3. What is a Semaphore?
4. Binary vs Counting Semaphore
5. Internal Working
6. Semaphore APIs
7. Important Arguments
8. Complete Program
9. Line-by-Line Explanation
10. Execution Flow
11. Internal Counter
12. Why Semaphore Has No Owner
13. Common Mistakes
14. Real World Use Cases
15. Interview Questions
16. Summary

---

# 1. Introduction

A semaphore is one of the oldest synchronization mechanisms.

Unlike a mutex,

it is **NOT designed primarily for protecting one critical section.**

Instead,

it controls

```
How many threads

can access

a resource simultaneously.
```

Think of a semaphore as

```
A Resource Counter
```

instead of

```
A Lock
```

---

# 2. Why Do We Need a Semaphore?

Suppose a company has

```
5 Database Connections
```

100 client threads arrive.

Should all 100 enter?

No.

Only

```
5

connections

exist.
```

Therefore

```
Semaphore Count = 5
```

First five threads

```
Allowed
```

Remaining

```
Wait
```

---

# 3. Real Life Analogy

Imagine a parking lot.

Capacity

```
3 Cars
```

Initially

```
Free Slots = 3
```

Cars arrive.

```
Car1

↓

Slot Taken

↓

Free Slots = 2
```

Car2

```
↓

Free Slots = 1
```

Car3

```
↓

Free Slots = 0
```

Car4

```
↓

Wait Outside
```

When Car2 leaves

```
Free Slots++

↓

Wake Car4

↓

Car4 Enters
```

This is exactly how a semaphore works.

---

# 4. Binary Semaphore vs Counting Semaphore

## Binary Semaphore

Counter

```
0 or 1
```

Looks similar to a mutex.

Example

```
sem = 1
```

Only one thread proceeds.

---

## Counting Semaphore

Counter

```
0

1

2

3

...

N
```

Example

```
sem = 10
```

Ten threads

can proceed simultaneously.

---

# 5. Internal Working

Conceptually

A semaphore stores

```
Counter

Waiting Queue
```

Example

Initially

```
Counter = 3

Queue = Empty
```

Thread1

calls

```
sem_wait()
```

Counter

```
3

↓

2
```

Allowed.

Thread2

```
2

↓

1
```

Allowed.

Thread3

```
1

↓

0
```

Allowed.

Thread4

```
Counter = 0

↓

Sleep

↓

Waiting Queue
```

---

# 6. Semaphore APIs

## sem_init()

Initializes a semaphore.

Prototype

```c
int sem_init(
    sem_t *sem,
    int pshared,
    unsigned int value
);
```

### Argument 1

```
&sem
```

Pointer to semaphore.

---

### Argument 2

```
pshared
```

0

```
Shared

between

Threads
```

Non-zero

```
Shared

between

Processes

(using shared memory)
```

---

### Argument 3

```
Initial Counter
```

Example

```c
sem_init(&sem,0,3);
```

Counter

starts from

```
3
```

---

## sem_wait()

Prototype

```c
sem_wait(&sem);
```

Meaning

```
Acquire one resource.
```

If

```
Counter > 0
```

Counter--

Continue.

If

```
Counter == 0
```

Thread sleeps.

---

## sem_post()

Prototype

```c
sem_post(&sem);
```

Meaning

```
Resource Released
```

Counter++

If

threads waiting

↓

Wake one.

---

## sem_destroy()

Destroys semaphore.

```c
sem_destroy(&sem);
```

---

# 7. Complete Example

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *worker(void *arg)
{
    sem_wait(&sem);

    printf("Thread %ld entered\n",(long)arg);

    sleep(2);

    printf("Thread %ld leaving\n",(long)arg);

    sem_post(&sem);

    return NULL;
}

int main()
{
    pthread_t t[4];

    sem_init(&sem,0,2);

    for(long i=0;i<4;i++)
        pthread_create(&t[i],NULL,worker,(void*)i);

    for(int i=0;i<4;i++)
        pthread_join(t[i],NULL);

    sem_destroy(&sem);

    return 0;
}
```

---

# 8. Step-by-Step Execution

Initial

```
Counter = 2
```

Thread0

```
sem_wait()

↓

Counter = 1

↓

Runs
```

Thread1

```
Counter = 0

↓

Runs
```

Thread2

```
Counter = 0

↓

Blocked
```

Thread3

```
Blocked
```

Thread0 finishes

```
sem_post()

↓

Counter++

↓

Wake Thread2
```

---

# 9. Why Semaphore Has No Owner

This is one of the biggest differences from a mutex.

Mutex

```
Thread1

Locks

↓

ONLY

Thread1

Unlocks
```

Semaphore

```
Thread1

sem_wait()

↓

Thread2

sem_post()
```

Perfectly valid.

Because

```
Semaphore

is

NOT

owned

by

any thread.
```

It only tracks

```
Available Resources
```

---

# 10. Internal Counter Example

Initial

```
Counter = 5
```

Five threads

```
↓

Counter

4

↓

3

↓

2

↓

1

↓

0
```

Sixth thread

```
↓

Wait
```

When

one thread

calls

```
sem_post()
```

Counter

```
1
```

Kernel

```
Wake

one

waiting

thread
```

---

# 11. Common Mistakes

### Mistake 1

Thinking

Semaphore

=

Mutex

Wrong.

---

### Mistake 2

Forgetting

```
sem_post()
```

Result

```
Counter

never increases

Threads wait forever.
```

---

### Mistake 3

Initializing

```
sem=0
```

and expecting

threads to run.

They will immediately block.

---

# 12. Real World Use Cases

### Database Connection Pool

Only

```
20

connections
```

exist.

Semaphore

```
20
```

---

### Printer Pool

Office

```
4 Printers
```

Semaphore

```
4
```

---

### GPU Resource

Suppose

GPU

supports

```
2

parallel jobs
```

Semaphore

```
2
```

---

### Producer Consumer

Count

```
Empty Slots

Filled Slots
```

using semaphores.

---

### Thread Pool

Maximum

```
8

Worker Threads
```

Semaphore limits

parallel execution.

---

# 13. Interview Questions

### What is a semaphore?

A semaphore is a synchronization primitive that controls access to a limited number of resources using an internal counter.

---

### Why is it called a counting semaphore?

Because it maintains a count of available resources.

---

### Can a semaphore replace a mutex?

Sometimes, but generally no.

A mutex provides ownership and is intended for mutual exclusion.

A semaphore is intended for counting resources or signaling.

---

### Can one thread call sem_wait() and another call sem_post()?

Yes.

Semaphores have no ownership.

---

### Binary Semaphore vs Mutex?

Binary Semaphore

```
Counter

0 or 1

No Ownership
```

Mutex

```
Owner Exists

Only owner unlocks.
```

---

# 14. Summary

| Feature | Semaphore |
|----------|-----------|
| Purpose | Count available resources |
| Owner | No |
| Waiting | Sleep |
| Counter | Yes |
| Multiple Threads Allowed | Yes |
| Can Signal Events | Yes |
| Can Synchronize Processes | Yes (with process-shared support) |

---

# Key Points to Remember

- A semaphore is **not just a lock**; it represents the number of available resources.
- `sem_wait()` decrements the counter or blocks if no resources are available.
- `sem_post()` increments the counter and may wake a waiting thread.
- Unlike a mutex, a semaphore has **no owner**.
- Counting semaphores are ideal for limiting concurrent access to a fixed set of identical resources.

---

End of Chapter 3
