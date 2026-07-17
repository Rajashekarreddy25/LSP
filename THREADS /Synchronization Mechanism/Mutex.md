# Linux System Programming Handbook

# Chapter 2
# Mutex (Mutual Exclusion)

---

# Table of Contents

1. Introduction
2. Why Mutex is Needed
3. What is a Mutex?
4. Internal Working of Mutex
5. Mutex States
6. Mutex APIs
7. Complete Example
8. Line-by-Line Code Explanation
9. Step-by-Step Execution
10. What Happens Inside pthread_mutex_lock()
11. Mutex Attributes
12. Types of Mutexes
13. Common Mistakes
14. Real World Use Cases
15. Interview Questions
16. Summary

---

# 1. Introduction

Mutex stands for

```
Mutual Exclusion
```

The name itself tells its purpose.

```
Mutual

↓

Only One

↓

Exclusive Access
```

Imagine there is only **one key** to a room.

Whoever has the key can enter.

Everyone else must wait.

Exactly the same happens with a mutex.

---

# 2. Why Do We Need a Mutex?

Suppose two threads access

```c
int counter = 0;
```

Thread 1

```c
counter++;
```

Thread 2

```c
counter++;
```

Without synchronization

```
Thread1

Read Counter

↓

Scheduler

↓

Thread2

Read Counter

↓

Increment

↓

Write

↓

Scheduler

↓

Thread1

Increment

↓

Write
```

One increment is lost.

This is called a

```
Race Condition
```

Mutex prevents this.

---

# 3. What is a Mutex?

A mutex is a synchronization primitive that allows

```
ONLY ONE THREAD

to execute

the critical section.
```

Everyone else waits.

Visual representation

```
             Mutex

        +------------+
        |  UNLOCKED  |
        +------------+

Thread1

Lock

↓

Critical Section

↓

Unlock

↓

Thread2 enters
```

---

# 4. Internal Working of a Mutex

Conceptually, a mutex contains something like

```
Mutex

----------------------

Lock State

Owner Thread

Waiting Queue

Attributes

----------------------
```

Example

Initially

```
Locked = No

Owner = NULL

Waiting Queue = Empty
```

Thread1

```
pthread_mutex_lock()
```

Now

```
Locked = Yes

Owner = Thread1

Waiting Queue = Empty
```

Thread2

tries to lock

Now

```
Locked = Yes

Owner = Thread1

Waiting Queue

↓

Thread2
```

Thread2 is blocked.

---

# 5. Mutex States

A mutex has only two logical states.

```
UNLOCKED
```

or

```
LOCKED
```

Initially

```
UNLOCKED
```

Thread1

```
lock()

↓

LOCKED
```

Thread2

```
lock()

↓

Sleep
```

Thread1

```
unlock()

↓

Wake Thread2

↓

Thread2 acquires mutex
```

---

# 6. Mutex APIs

## pthread_mutex_init()

Initializes a mutex.

Syntax

```c
pthread_mutex_init(
        pthread_mutex_t *mutex,
        const pthread_mutexattr_t *attr);
```

### First Argument

```
&mutex
```

Pointer to the mutex object.

### Second Argument

```
NULL
```

Uses default mutex attributes.

---

Example

```c
pthread_mutex_t mutex;

pthread_mutex_init(&mutex,NULL);
```

---

## pthread_mutex_lock()

Locks the mutex.

Syntax

```c
pthread_mutex_lock(&mutex);
```

If unlocked

```
Acquire immediately.
```

If already locked

```
Thread sleeps.
```

---

## pthread_mutex_unlock()

Releases the mutex.

Syntax

```c
pthread_mutex_unlock(&mutex);
```

Only the thread that owns the mutex should unlock it.

---

## pthread_mutex_destroy()

Destroys the mutex.

```c
pthread_mutex_destroy(&mutex);
```

Destroy it only after no thread is using it.

---

# 7. Complete Example

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;

pthread_mutex_t mutex;

void *worker(void *arg)
{
    for(int i=0;i<5;i++)
    {
        pthread_mutex_lock(&mutex);

        counter++;

        printf("Counter = %d\n",counter);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main()
{
    pthread_t t1,t2;

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&t1,NULL,worker,NULL);

    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);

    pthread_join(t2,NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
```

---

# 8. Line-by-Line Explanation

## Global Variable

```c
int counter=0;
```

Shared by both threads.

This is the shared resource.

---

## Mutex

```c
pthread_mutex_t mutex;
```

Creates a mutex object.

Initially

```
Unlocked
```

---

## Initialize

```c
pthread_mutex_init(&mutex,NULL);
```

Creates a usable mutex.

---

## Lock

```c
pthread_mutex_lock(&mutex);
```

Suppose Thread1 executes.

Mutex

```
Unlocked

↓

Locked

↓

Owner = Thread1
```

---

## Critical Section

```c
counter++;
```

Only Thread1 executes it.

Thread2 must wait.

---

## Unlock

```c
pthread_mutex_unlock(&mutex);
```

Mutex becomes

```
Unlocked
```

Kernel wakes one waiting thread.

---

# 9. Step-by-Step Execution

Initially

```
counter = 0

Mutex = Unlocked
```

---

Thread1

```
Lock

↓

Success

↓

Counter = 1

↓

Unlock
```

---

Thread2

```
Lock

↓

Success

↓

Counter = 2

↓

Unlock
```

---

Suppose Thread2 arrives first.

```
Thread2

Lock

↓

Thread1 tries Lock

↓

Blocked

↓

Sleeping
```

CPU executes another runnable thread.

Thread2

```
Unlock
```

Kernel wakes Thread1.

---

# 10. What Happens Inside pthread_mutex_lock()?

This is a conceptual sequence (the real implementation is optimized and uses atomic operations and the Linux futex mechanism).

```
Thread

↓

Call pthread_mutex_lock()

↓

Is mutex unlocked?

↓

YES

↓

Acquire mutex

↓

Continue
```

Otherwise

```
Already Locked?

↓

YES

↓

Place thread in wait queue

↓

Thread state = Blocked

↓

Scheduler selects another runnable thread
```

Later

```
Owner unlocks mutex

↓

Kernel wakes one waiting thread

↓

Thread acquires mutex

↓

Returns from pthread_mutex_lock()
```

---

# 11. Thread State Changes

Suppose

Thread1

owns mutex.

Thread2

tries

```
pthread_mutex_lock()
```

Thread2 becomes

```
RUNNING

↓

BLOCKED

↓

WAITING

↓

READY

↓

RUNNING
```

The scheduler manages these transitions.

---

# 12. Mutex Types

Linux provides different mutex behaviors through attributes.

### Normal Mutex

Default mutex.

```
Fastest

No error checking
```

---

### Error Checking Mutex

Detects mistakes like

```
Locking same mutex twice

Unlocking without ownership
```

Useful during debugging.

---

### Recursive Mutex

Allows the same thread to lock the same mutex multiple times.

Example

```
Thread1

lock

↓

lock again

↓

lock again

↓

unlock

↓

unlock

↓

unlock
```

The mutex keeps an internal recursion count.

Without a recursive mutex, the second lock attempt would block forever because the thread would be waiting on a lock it already owns.

---

# 13. Common Mistakes

## Forgetting to Unlock

```c
pthread_mutex_lock(&mutex);

return;
```

Forgot

```c
pthread_mutex_unlock(&mutex);
```

Every other thread waits forever.

---

## Double Lock

```c
pthread_mutex_lock(&mutex);

pthread_mutex_lock(&mutex);
```

Using a normal mutex, the thread blocks waiting for itself.

---

## Unlocking from Another Thread

```
Thread1

Locks

↓

Thread2

Unlocks
```

This is incorrect and results in undefined behavior.

---

## Locking Too Much Code

Bad

```c
pthread_mutex_lock(&mutex);

sleep(5);

counter++;

printf("%d",counter);

pthread_mutex_unlock(&mutex);
```

The thread holds the mutex unnecessarily long.

Better

```c
pthread_mutex_lock(&mutex);

counter++;

pthread_mutex_unlock(&mutex);

printf("%d",counter);
```

Keep the critical section as short as possible.

---

# 14. Real World Use Cases

### Shared Counter

```
Total Requests

Total Connections

Packets Received
```

---

### Bank Account

```
Deposit

Withdraw

Balance
```

---

### Linked List

```
Insert

Delete

Modify
```

---

### Queue

```
Push

Pop
```

---

### Log File

Only one thread writes to the log at a time.

---

### Device Driver

Protect shared driver state when accessed by multiple execution contexts.

---

# 15. Interview Questions

## What is a mutex?

A mutex is a synchronization mechanism that provides mutual exclusion, allowing only one thread at a time to execute a critical section.

---

## Why is a mutex needed?

To prevent race conditions when multiple threads access shared resources.

---

## Does a mutex consume CPU while waiting?

No.

A waiting thread is blocked and does not busy-wait.

---

## Can another thread unlock my mutex?

No.

The thread that locks the mutex should be the one that unlocks it.

---

## Can multiple processes use the same mutex?

Yes, but only if:

- The mutex is placed in shared memory.
- It is initialized with the `PTHREAD_PROCESS_SHARED` attribute.

---

## Mutex vs Spinlock

Mutex

```
Wait

↓

Sleep

↓

Wake later
```

Spinlock

```
Wait

↓

Keep checking

↓

CPU busy
```

---

# 16. Summary

| Feature | Mutex |
|----------|-------|
| Purpose | Protect shared data |
| Maximum Owners | One |
| Waiting Method | Sleeping |
| CPU Usage | Low while waiting |
| Ownership | Yes |
| Used In | User-space and kernel (appropriate mutex APIs) |
| Best For | Long or moderate critical sections |

---

# Key Points to Remember

- A mutex protects a **critical section**.
- Only one thread owns a mutex at a time.
- Waiting threads are **blocked**, not spinning.
- The owner should unlock the mutex.
- Keep critical sections as short as practical.
- Always pair every successful `pthread_mutex_lock()` with a corresponding `pthread_mutex_unlock()`.

---

End of Chapter 2
