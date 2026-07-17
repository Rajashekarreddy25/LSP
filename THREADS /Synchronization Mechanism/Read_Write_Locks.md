# Linux System Programming Handbook

# Chapter 8
# Read-Write Locks (pthread_rwlock)

---

# Table of Contents

1. Why Mutex is Sometimes Inefficient
2. The Reader Problem
3. What is a Read-Write Lock?
4. Internal Working
5. Reader Lock vs Writer Lock
6. Reader Preference vs Writer Preference
7. pthread_rwlock APIs
8. Complete Program
9. Step-by-Step Execution
10. Internal State Transitions
11. Starvation
12. Real Linux Use Cases
13. Mutex vs RW Lock vs Spinlock
14. Common Mistakes
15. Interview Questions
16. Summary

---

# 1. Why Mutex is Sometimes Inefficient?

Let's start with a simple question.

Suppose 100 threads only want to **read** a shared variable.

Example

```c
int temperature = 30;
```

Thread1

```c
printf("%d", temperature);
```

Thread2

```c
printf("%d", temperature);
```

Thread3

```c
printf("%d", temperature);
```

Nobody is modifying the variable.

Question:

Should only **one** thread be allowed to read?

No.

Reading does not change the value.

Yet if we use a mutex:

```
Reader1

↓

Lock

↓

Read

↓

Unlock

↓

Reader2

↓

Lock

↓

Read

↓

Unlock
```

Every reader waits.

This is unnecessary.

---

# 2. The Reader Problem

Imagine a library.

100 students want to **read** the same book.

Should only one student be allowed?

No.

Now imagine someone wants to **edit** the book.

While editing,

nobody else should read it.

This is exactly the idea behind a Read-Write Lock.

---

# 3. What is a Read-Write Lock?

A Read-Write Lock allows:

```
Multiple Readers

OR

One Writer
```

But never both simultaneously.

Rules:

```
Readers + Readers

✔ Allowed
```

```
Readers + Writer

✘ Not Allowed
```

```
Writer + Writer

✘ Not Allowed
```

---

# 4. Visual Representation

Initially

```
RW Lock

↓

Free
```

Reader1

```
Read Lock

↓

Allowed
```

Reader2

```
Read Lock

↓

Allowed
```

Reader3

```
Read Lock

↓

Allowed
```

Writer arrives

```
Wait
```

because readers are active.

When the last reader leaves

Writer acquires the lock.

---

# 5. Internal Working

Conceptually,

a read-write lock maintains information such as:

```
Reader Count

Writer Active?

Waiting Readers

Waiting Writers
```

Example

Initially

```
Readers = 0

Writer = No
```

Reader1

```
Readers = 1
```

Reader2

```
Readers = 2
```

Reader3

```
Readers = 3
```

Writer arrives

```
Writer Waiting
```

Only after

```
Readers = 0
```

does the writer proceed.

---

# 6. Reader Lock vs Writer Lock

Read Lock

```c
pthread_rwlock_rdlock(&rwlock);
```

Meaning

```
I only want to READ.
```

Multiple readers may hold this lock.

---

Write Lock

```c
pthread_rwlock_wrlock(&rwlock);
```

Meaning

```
I want to MODIFY data.
```

Only one writer is allowed.

---

Unlock

```c
pthread_rwlock_unlock(&rwlock);
```

Releases either a read or write lock.

---

# 7. Reader Preference vs Writer Preference

Different implementations may favor readers or writers.

### Reader Preference

Readers can continue entering while readers are already active.

Problem:

```
Writer

↓

Waiting

↓

New Reader

↓

Allowed

↓

Writer Still Waiting
```

Writer may wait a long time.

---

### Writer Preference

When a writer starts waiting,

new readers are held back.

Timeline

```
Reader1

↓

Reader2

↓

Writer Arrives

↓

No New Readers

↓

Readers Finish

↓

Writer Runs
```

This reduces writer starvation.

Implementation details vary across systems and libraries.

---

# 8. RW Lock APIs

Initialize

```c
pthread_rwlock_init(
    &rwlock,
    NULL);
```

Read Lock

```c
pthread_rwlock_rdlock(&rwlock);
```

Write Lock

```c
pthread_rwlock_wrlock(&rwlock);
```

Unlock

```c
pthread_rwlock_unlock(&rwlock);
```

Destroy

```c
pthread_rwlock_destroy(&rwlock);
```

---

# 9. Example Program

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock;

int value = 100;

void *reader(void *arg)
{
    pthread_rwlock_rdlock(&rwlock);

    printf("Reader %ld: %d\n",
            (long)arg,
            value);

    sleep(2);

    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

void *writer(void *arg)
{
    pthread_rwlock_wrlock(&rwlock);

    value += 10;

    printf("Writer Updated Value = %d\n",
            value);

    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

int main()
{
    pthread_t r1,r2,r3,w;

    pthread_rwlock_init(&rwlock,NULL);

    pthread_create(&r1,NULL,reader,(void*)1);
    pthread_create(&r2,NULL,reader,(void*)2);
    pthread_create(&r3,NULL,reader,(void*)3);

    sleep(1);

    pthread_create(&w,NULL,writer,NULL);

    pthread_join(r1,NULL);
    pthread_join(r2,NULL);
    pthread_join(r3,NULL);
    pthread_join(w,NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
```

---

# 10. Step-by-Step Execution

Initial

```
Value = 100
```

Reader1

```
Read Lock

↓

Allowed
```

Reader2

```
Read Lock

↓

Allowed
```

Reader3

```
Read Lock

↓

Allowed
```

All readers execute together.

Now Writer

tries

```
Write Lock
```

Blocked.

Why?

Readers are active.

Readers finish.

```
Reader Count

3

↓

2

↓

1

↓

0
```

Writer

```
Write Lock

↓

Allowed

↓

Modify Value

↓

Unlock
```

---

# 11. State Diagram

```
                Free
                 |
        +--------+--------+
        |                 |
   Reader Lock       Writer Lock
        |                 |
 Readers Count++      Writer Active
        |                 |
 Readers Unlock      Writer Unlock
        |                 |
        +--------+--------+
                 |
                Free
```

---

# 12. Starvation

### Reader Starvation

Rare in reader-preference implementations.

---

### Writer Starvation

Imagine

```
Reader

↓

Reader

↓

Reader

↓

Reader

↓

Reader
```

New readers keep arriving.

Writer waits indefinitely.

To avoid this,

many implementations give waiting writers priority over new readers.

---

# 13. Real Linux Use Cases

Routing Table

```
Thousands of Reads

Few Updates
```

---

DNS Cache

```
Many Reads

Occasional Writes
```

---

Configuration Data

```
Read Frequently

Modified Rarely
```

---

Filesystem Metadata

---

Kernel Data Structures

Some kernel subsystems use read-write synchronization primitives (often kernel-specific APIs rather than POSIX `pthread_rwlock_t`) when read operations greatly outnumber writes.

---

# 14. Mutex vs RW Lock vs Spinlock

| Feature | Mutex | RW Lock | Spinlock |
|----------|--------|----------|-----------|
| Multiple Readers | No | Yes | No |
| One Writer | Yes | Yes | Yes |
| Waiting | Sleep | Sleep | Busy Wait |
| CPU Usage While Waiting | Low | Low | High |
| Best For | General Shared Data | Read-Mostly Data | Very Short Critical Sections |

---

# 15. Common Mistakes

Mistake

Using

```
Read Lock

↓

Modify Data
```

Wrong.

If you modify data,

always use

```
Write Lock
```

---

Mistake

Using RW Lock

when

writes happen continuously.

In that case,

a mutex is often simpler and may perform just as well or better.

---

Mistake

Holding write lock during

```
sleep()

File I/O

Network

Database
```

Avoid long write critical sections.

---

# 16. Interview Questions

Q.

Why use RW Lock instead of Mutex?

Answer

Because multiple readers can execute simultaneously,

improving throughput when reads greatly outnumber writes.

---

Q.

Can two writers execute together?

No.

Only one writer may hold the write lock.

---

Q.

Can readers and writers execute together?

No.

Writers require exclusive access.

---

Q.

Can RW Locks deadlock?

Yes.

Improper lock ordering or upgrading/downgrading strategies can still lead to deadlocks.

---

Q.

When should RW Locks NOT be used?

When writes are frequent,

or the protected critical section is very small.

---

# Summary

| Feature | Read-Write Lock |
|----------|-----------------|
| Readers | Multiple |
| Writers | One |
| Reader + Writer Together | No |
| Wait Method | Sleep |
| Best Use Case | Read-Mostly Data |
| Performance | Better than Mutex for Read-Heavy Workloads |

---

# Key Points to Remember

✓ Multiple readers can access data simultaneously.

✓ Writers always require exclusive access.

✓ RW Locks improve scalability for read-heavy workloads.

✓ They are not automatically better than mutexes—choose them based on the read/write ratio.

✓ Long write critical sections reduce their benefit.

---

End of Chapter 8
