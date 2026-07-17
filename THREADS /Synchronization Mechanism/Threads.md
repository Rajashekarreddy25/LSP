# Linux System Programming Handbook

# Chapter 1
# Introduction to Threads, Synchronization, Critical Section and Race Condition

---

# Table of Contents

1. Why do we need synchronization?
2. Process vs Thread
3. Shared Resources
4. Critical Section
5. Race Condition
6. Critical Section Solution
7. Example Programs
8. Execution Flow
9. Common Mistakes
10. Interview Questions
11. Summary

---

# 1. Why Do We Need Synchronization?

Modern computers execute multiple tasks simultaneously.

Examples:

- Browser downloading files
- Music player
- Video player
- Bluetooth service
- WiFi service

Even a simple application may create multiple threads.

Example:

```
Browser

│

├── UI Thread

├── Download Thread

├── Network Thread

├── Rendering Thread

└── Cache Thread
```

These threads often share data.

Examples:

```
Shared Counter

Shared Queue

Shared Linked List

Shared Log File

Shared Memory
```

When multiple threads access the same resource simultaneously,
unexpected behavior can occur.

This is why synchronization is needed.

---

# 2. Process vs Thread

## Process

A process has its own address space.

```
Process A

Code

Data

Heap

Stack
```

Another process

```
Process B

Code

Data

Heap

Stack
```

They cannot directly access each other's memory.

---

## Thread

Threads belong to the same process.

```
Process

----------------------

Heap

Global Variables

Open Files

Shared Memory

----------------------

Thread1 Stack

Thread2 Stack

Thread3 Stack
```

Threads share

- Global variables
- Heap memory
- Files
- Sockets

Each thread has

- Program Counter
- Registers
- Stack

Because threads share memory,
synchronization becomes necessary.

---

# 3. Shared Resource

A shared resource is anything that multiple threads can access.

Examples

```
Global Variable

File

Socket

Queue

Database Connection

Hardware Register

Linked List
```

Example

```c
int counter = 0;
```

Thread1

```
counter++
```

Thread2

```
counter++
```

Both access the same variable.

This variable is a shared resource.

---

# 4. Critical Section

## Definition

A critical section is the part of the program that accesses shared resources.

Only ONE thread should execute it at a time.

Example

```c
counter++;
```

Looks like one instruction.

Actually it is

```
Read counter

↓

Increment

↓

Write counter
```

These are multiple CPU operations.

---

## Example

```c
counter = counter + 1;
```

CPU executes

```
Load counter

↓

Add 1

↓

Store counter
```

Another thread may interrupt in between.

---

# Example without Synchronization

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;

void *worker(void *arg)
{
    for(int i=0;i<100000;i++)
    {
        counter++;
    }

    return NULL;
}

int main()
{
    pthread_t t1,t2;

    pthread_create(&t1,NULL,worker,NULL);
    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    printf("Counter = %d\n",counter);
}
```

Expected Output

```
200000
```

Possible Output

```
178654

192563

199876

183445
```

Every execution may produce a different value.

---

# Why?

Suppose

```
counter = 5
```

Thread1

```
Read 5
```

Scheduler switches.

Thread2

```
Read 5

Increment

Write 6
```

Scheduler switches.

Thread1

```
Increment

Write 6
```

Expected

```
7
```

Actual

```
6
```

One increment is lost.

This is called a race condition.

---

# 5. Race Condition

## Definition

A race condition occurs when

- Multiple threads access shared data
- At least one modifies the data
- There is no synchronization
- The result depends on execution timing

---

Example

```
Balance = 1000
```

Thread1

Withdraw 500

Thread2

Withdraw 700

Both read

```
1000
```

Both calculate independently.

Final balance becomes incorrect.

---

Another Example

```
Shared Log File
```

Thread1

```
Hello
```

Thread2

```
World
```

Possible Output

```
HelWorlldo
```

because both write simultaneously.

---

# 6. Solving Race Condition

The simplest solution is using a Mutex.

```
pthread_mutex_lock()

Critical Section

pthread_mutex_unlock()
```

Now

```
Only one thread

↓

Enters Critical Section

↓

Leaves

↓

Next Thread Enters
```

---

# Protected Example

```c
#include <stdio.h>
#include <pthread.h>

int counter=0;

pthread_mutex_t mutex;

void *worker(void *arg)
{
    for(int i=0;i<100000;i++)
    {
        pthread_mutex_lock(&mutex);

        counter++;

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

    printf("%d\n",counter);

    pthread_mutex_destroy(&mutex);
}
```

Now output is always

```
200000
```

---

# Execution Flow

Without Mutex

```
Thread1

Read

↓

Scheduler

↓

Thread2

Read

Increment

Write

↓

Scheduler

↓

Thread1

Increment

Write
```

Race Condition.

---

With Mutex

```
Thread1

Lock

↓

Increment

↓

Unlock

↓

Thread2

Lock

↓

Increment

↓

Unlock
```

Safe.

---

# Common Mistakes

### Mistake 1

Thinking

```
counter++
```

is one CPU instruction.

It is NOT.

---

### Mistake 2

Believing

```
printf()
```

is thread-safe.

Output from multiple threads can interleave.

---

### Mistake 3

Protecting only half of a critical section.

Wrong

```c
pthread_mutex_lock(&mutex);

counter++;

pthread_mutex_unlock(&mutex);

printf("%d",counter);
```

If the print is intended to reflect the update atomically with the increment, it should also be protected.

---

# Interview Questions

Q1. What is a critical section?

Answer

A critical section is the portion of code that accesses shared resources and therefore must not be executed simultaneously by multiple threads.

---

Q2. What is a race condition?

Answer

A race condition occurs when multiple threads access shared data without synchronization and the program's output depends on execution timing.

---

Q3. How do you solve a race condition?

Answer

Using synchronization primitives such as

- Mutex
- Semaphore
- Spinlock
- Read-Write Lock
- Atomic Operations

depending on the situation.

---

# Summary

| Topic | Description |
|--------|-------------|
| Process | Independent address space |
| Thread | Shares memory with other threads |
| Shared Resource | Resource accessed by multiple threads |
| Critical Section | Code accessing shared data |
| Race Condition | Incorrect behavior due to concurrent unsynchronized access |
| Solution | Synchronization primitives |

---

End of Chapter 1





