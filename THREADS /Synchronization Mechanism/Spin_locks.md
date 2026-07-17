# Linux System Programming Handbook

# Chapter 6
# Spinlocks (pthread_spinlock & Linux Kernel Spinlocks)

---

# Table of Contents

1. Why Spinlocks Exist
2. Mutex vs Spinlock
3. What is Busy Waiting?
4. Internal Working
5. Why Sleeping Can Be Expensive
6. When Spinlocks Should Be Used
7. When Spinlocks Should NOT Be Used
8. pthread Spinlock APIs
9. Code Example
10. Step-by-Step Execution
11. Linux Kernel Spinlocks
12. Interrupt Context
13. Common Mistakes
14. Interview Questions
15. Summary

---

# 1. Why Spinlocks Exist?

Suppose Thread A locks a mutex.

Thread B tries

```
pthread_mutex_lock()
```

Thread B

```
↓

Blocked

↓

Scheduler Runs

↓

Context Switch

↓

Sleep

↓

Wake Later
```

Everything looks fine.

But...

Suppose Thread A releases the lock after only

```
100 nanoseconds
```

The scheduler spent

```
Several microseconds

putting Thread B to sleep

and

waking it again.
```

Sleeping took much longer than simply waiting.

This is where spinlocks help.

---

# 2. What is a Spinlock?

A spinlock is a synchronization primitive that protects a critical section **by repeatedly checking (spinning) until the lock becomes available**.

Instead of

```
Sleep
```

it does

```
Try

Try Again

Try Again

Try Again

Try Again
```

continuously.

---

# 3. Busy Waiting

Imagine

```
Door Locked
```

Mutex

```
Door Locked

↓

Go Home

↓

Someone Calls You

↓

Come Back
```

Spinlock

```
Door Locked

↓

Stand

Outside

Checking

Again

Again

Again

Again
```

This is called

```
Busy Waiting
```

---

# 4. Internal Working

Conceptually

Spinlock contains

```
Locked ?

Owner

Atomic Variable
```

Initially

```
Locked = No
```

Thread1

```
spin_lock()

↓

Locked = Yes
```

Thread2

tries

```
spin_lock()
```

Internally

```
while(lock == BUSY)
{
    // keep checking
}
```

When Thread1 unlocks

```
Lock = Free
```

Thread2 immediately acquires it.

---

# 5. Why Sleeping Can Be Expensive

Sleeping is not free.

Suppose

```
Thread2

↓

Calls pthread_mutex_lock()

↓

Kernel

↓

Context Switch

↓

Scheduler

↓

Sleep

↓

Wake

↓

Scheduler Again

↓

Run Thread
```

Many operations occur.

If the lock is held for

```
100 ns
```

Sleeping is slower.

Spinlock simply waits.

---

# 6. Mutex vs Spinlock Timeline

Mutex

```
Thread2

↓

Lock Busy

↓

Sleep

↓

Wake Later

↓

Continue
```

Spinlock

```
Thread2

↓

Lock Busy

↓

Keep Checking

↓

Acquire Immediately
```

---

# 7. CPU Usage

Mutex

```
Waiting

↓

0% CPU
```

Spinlock

```
Waiting

↓

100% CPU
```

This is the biggest disadvantage.

---

# 8. When Should We Use Spinlocks?

Only when

```
Critical Section

is

VERY SHORT
```

Examples

```
50 ns

100 ns

200 ns
```

If lock duration is longer,

spinlocks waste CPU.

---

# 9. When Should We NOT Use Spinlocks?

Suppose

```
spin_lock();

sleep(5);

spin_unlock();
```

Terrible.

Other CPUs

spin

for

5 seconds.

CPU usage

```
100%
```

Never hold a spinlock while doing slow operations.

---

# 10. pthread Spinlock APIs

Initialize

```c
pthread_spin_init(
        &lock,
        PTHREAD_PROCESS_PRIVATE);
```

Acquire

```c
pthread_spin_lock(&lock);
```

Release

```c
pthread_spin_unlock(&lock);
```

Destroy

```c
pthread_spin_destroy(&lock);
```

---

# 11. Important Arguments

## pthread_spin_init()

```c
pthread_spin_init(
        &lock,
        PTHREAD_PROCESS_PRIVATE);
```

First Argument

```
Spinlock Object
```

Second Argument

```
PTHREAD_PROCESS_PRIVATE
```

Meaning

Only

threads

inside

same process.

There is also

```
PTHREAD_PROCESS_SHARED
```

for process-shared spinlocks if supported by the implementation.

---

# 12. Example Program

```c
#include <stdio.h>
#include <pthread.h>

pthread_spinlock_t lock;

int counter = 0;

void *worker(void *arg)
{
    for(int i=0;i<5;i++)
    {
        pthread_spin_lock(&lock);

        counter++;

        printf("%d\n",counter);

        pthread_spin_unlock(&lock);
    }

    return NULL;
}

int main()
{
    pthread_t t1,t2;

    pthread_spin_init(
        &lock,
        PTHREAD_PROCESS_PRIVATE);

    pthread_create(&t1,NULL,worker,NULL);

    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);

    pthread_join(t2,NULL);

    pthread_spin_destroy(&lock);

    return 0;
}
```

---

# 13. Step-by-Step Execution

Initially

```
Lock = Free
```

Thread1

```
spin_lock()

↓

Success

↓

Counter++

↓

spin_unlock()
```

Thread2

tries

```
spin_lock()
```

Suppose Thread1 still owns it.

Thread2

```
while(lock busy)
{

}

↓

Immediately Acquires

↓

Counter++
```

No sleeping.

---

# 14. Linux Kernel Spinlocks

Spinlocks are extremely common in the Linux kernel.

Examples

```
Scheduler

Run Queue

Task List

Interrupt Handling

DMA Descriptor

Device Driver

Network Driver

USB Driver

Bluetooth Driver
```

---

# 15. Why Kernel Loves Spinlocks

Suppose

Interrupt Handler

executes.

Can it call

```
sleep() ?
```

NO.

Because

Interrupt Context

has

NO PROCESS

to sleep.

Therefore

Kernel uses

```
Spinlocks
```

instead.

---

# 16. Interrupt Context

Imagine

CPU

```
Running Process
```

Suddenly

```
Keyboard Interrupt
```

Interrupt Handler starts.

Suppose

Handler tries

```
pthread_mutex_lock()
```

Impossible.

Mutex

may sleep.

Interrupt Handler

cannot sleep.

Therefore

Linux Kernel provides

```
spin_lock()
```

for interrupt-safe synchronization.

---

# 17. Kernel APIs

Kernel

does NOT use

```
pthread_spin_lock()
```

Instead

```
spin_lock()

spin_unlock()

spin_lock_irq()

spin_unlock_irq()

spin_lock_irqsave()

spin_unlock_irqrestore()
```

These are kernel APIs.

---

# 18. Common Mistakes

Mistake

Using spinlock

around

```
printf()

malloc()

sleep()

disk access

network access
```

Wrong.

Spinlocks should protect only very short operations.

---

Mistake

Thinking

Spinlock

is

faster

always.

Wrong.

If lock duration is long,

mutex is faster because waiting threads sleep instead of wasting CPU.

---

Mistake

Using spinlock

for file operations.

Files are slow.

Use mutex instead.

---

# 19. Real World Use Cases

Linux Scheduler

```
Run Queue
```

Network Driver

```
Packet Queue
```

Bluetooth Driver

```
HCI Queue
```

Camera Driver

```
DMA Descriptor
```

USB Driver

```
Transfer Queue
```

Kernel Memory Allocator

```
Small Critical Sections
```

---

# 20. Interview Questions

Q.

Why are spinlocks used?

Answer

To protect very short critical sections where the cost of putting a thread to sleep and waking it later is greater than simply busy waiting.

---

Q.

Why are spinlocks common in Linux kernel?

Answer

Because many kernel contexts, such as interrupt handlers, are not allowed to sleep.

---

Q.

Spinlock vs Mutex?

Mutex

```
Sleep

↓

Low CPU Usage

↓

Long Critical Section
```

Spinlock

```
Busy Wait

↓

High CPU Usage

↓

Short Critical Section
```

---

Q.

Can spinlocks cause deadlock?

Yes.

Just like mutexes.

If locks are acquired in inconsistent order,

deadlock is still possible.

---

# Summary

| Feature | Spinlock |
|----------|----------|
| Waiting Method | Busy Waiting |
| Sleeps | No |
| CPU Usage | High while waiting |
| Critical Section | Very Short |
| Used Mostly | Linux Kernel |
| Interrupt Safe | Yes (kernel spinlock APIs) |
| Good For | Short lock durations |
| Bad For | Long operations |

---

# Key Points to Remember

✓ Spinlocks protect critical sections by busy waiting.

✓ They should be held only for a very short time.

✓ Never sleep while holding a spinlock.

✓ Spinlocks are widely used inside the Linux kernel.

✓ User-space applications usually prefer mutexes unless profiling shows that a spinlock is appropriate.

---

End of Chapter 6
