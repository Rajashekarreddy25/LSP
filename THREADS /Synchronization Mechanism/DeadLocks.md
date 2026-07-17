# Linux System Programming Handbook

# Chapter 7
# Deadlocks (Complete Masterclass)

---

# Table of Contents

1. What is a Deadlock?
2. Real Life Analogy
3. Deadlock vs Race Condition
4. How Deadlocks Occur
5. Coffman Conditions
6. Two-Mutex Example
7. Internal Timeline
8. Why the Scheduler Cannot Fix It
9. Deadlock Prevention
10. Deadlock Avoidance
11. Deadlock Detection
12. trylock() and timedlock()
13. Lock Ordering
14. Common Mistakes
15. Debugging Deadlocks
16. Interview Questions
17. Summary

---

# 1. What is a Deadlock?

Definition

A deadlock occurs when two or more threads or processes are **permanently blocked**, each waiting for a resource held by another.

Unlike a race condition, a deadlock does **not** produce incorrect data.

Instead,

the program simply

```
Stops

Forever
```

without making progress.

---

# 2. Real-Life Analogy

Imagine a one-lane bridge.

```
Car A  ---> <---  Car B
```

Neither car can move forward.

Neither driver wants to reverse.

Result

```
Everyone Waits Forever
```

Another analogy:

Two people exchange keys.

```
Person A

Has Key 1

Needs Key 2
```

```
Person B

Has Key 2

Needs Key 1
```

Neither can continue.

---

# 3. Deadlock vs Race Condition

Race Condition

```
Multiple Threads

↓

Access Same Data

↓

Incorrect Result
```

Example

Expected

```
Counter = 200000
```

Actual

```
183742
```

---

Deadlock

```
Thread1 waits

↓

Thread2 waits

↓

Nobody Continues
```

No incorrect value.

Just

```
Program Hangs
```

---

# 4. Simplest Deadlock Example

Suppose we have

```c
pthread_mutex_t m1;
pthread_mutex_t m2;
```

Thread 1

```c
pthread_mutex_lock(&m1);

sleep(1);

pthread_mutex_lock(&m2);
```

Thread 2

```c
pthread_mutex_lock(&m2);

sleep(1);

pthread_mutex_lock(&m1);
```

Looks harmless.

But watch carefully.

---

# 5. Step-by-Step Execution

Initially

```
m1 = Free

m2 = Free
```

Thread1

```
Locks m1
```

State

```
m1 -> Thread1

m2 -> Free
```

Scheduler switches.

Thread2

```
Locks m2
```

State

```
m1 -> Thread1

m2 -> Thread2
```

Thread1 resumes.

```
Needs m2
```

But

```
m2 owned by Thread2
```

Thread1 sleeps.

Scheduler switches.

Thread2 resumes.

Needs

```
m1
```

But

```
m1 owned by Thread1
```

Thread2 sleeps.

Now

```
Thread1 Waiting for m2

↓

Thread2 Waiting for m1
```

Nobody can continue.

---

# 6. Internal Timeline

```
Time →

Thread1

Lock m1

↓

Wait m2

-------------------------

Thread2

          Lock m2

          ↓

          Wait m1
```

Both threads become

```
Blocked
```

The scheduler looks for runnable threads.

There are none.

---

# 7. Why Can't the Scheduler Fix It?

This is a common interview question.

Many beginners think:

> "The scheduler will wake one of them."

No.

The scheduler can only run **runnable** threads.

In a deadlock,

both threads are blocked waiting for events that **cannot occur**.

The scheduler cannot magically unlock a mutex.

---

# 8. Coffman Conditions

For a deadlock to occur,

all **four** of these conditions must exist simultaneously.

If **any one** of them is removed,

the deadlock cannot occur.

---

## Condition 1 - Mutual Exclusion

Only one thread can own the resource.

Example

```
Mutex
```

Only one owner.

If two threads could own it simultaneously,

deadlock would not occur.

---

## Condition 2 - Hold and Wait

Thread already owns one resource

AND

waits for another.

Example

```
Thread1

Owns m1

Waiting for m2
```

---

## Condition 3 - No Preemption

Resources cannot be forcibly taken away.

Example

Thread1 owns mutex.

Kernel cannot simply steal it.

Thread1 must unlock it voluntarily.

---

## Condition 4 - Circular Wait

```
Thread1 → m2

↓

Thread2 → m1

↓

Back to Thread1
```

Forms a cycle.

This circular dependency is the essence of deadlock.

---

# 9. Visual Representation

```
Thread1

↓

Needs m2

↑

m1

↓

Thread2

↓

Needs m1

↑

m2
```

Circular dependency.

---

# 10. Deadlock Prevention

The easiest way is to eliminate one Coffman condition.

---

## Method 1 - Lock Ordering

Always acquire locks in the same order.

Correct

Thread1

```
Lock m1

↓

Lock m2
```

Thread2

```
Lock m1

↓

Lock m2
```

Now,

Thread2 waits for m1 first.

Once Thread1 finishes,

Thread2 acquires both locks.

No cycle.

---

## Method 2 - Keep Critical Sections Short

Bad

```c
pthread_mutex_lock(&m1);

sleep(5);

pthread_mutex_unlock(&m1);
```

Good

```c
pthread_mutex_lock(&m1);

/* Only essential work */

pthread_mutex_unlock(&m1);
```

The less time a lock is held,

the lower the chance of deadlock and contention.

---

## Method 3 - Avoid Nested Locks

Instead of

```
Lock A

↓

Lock B
```

try to redesign so only one lock is needed at a time.

---

## Method 4 - Release Before Waiting

If a thread needs another resource,

sometimes it can release the current one first,

then retry later.

This removes the "Hold and Wait" condition.

---

# 11. Using pthread_mutex_trylock()

Unlike

```c
pthread_mutex_lock()
```

which blocks,

`pthread_mutex_trylock()` returns immediately.

Example

```c
if (pthread_mutex_trylock(&m2) != 0)
{
    pthread_mutex_unlock(&m1);

    /* Retry later */
}
```

Instead of waiting forever,

the thread backs off.

This can help avoid certain deadlock scenarios.

---

# 12. Using pthread_mutex_timedlock()

Wait only for a limited time.

```c
pthread_mutex_timedlock();
```

If timeout expires,

the function returns an error.

Program can

```
Retry

or

Recover
```

instead of hanging forever.

---

# 13. Deadlock Avoidance

Prevention

```
Never Allow Deadlock
```

Avoidance

```
Check

before

allocating

resource
```

A famous avoidance algorithm is

```
Banker's Algorithm
```

It is important academically,

but rarely used directly in Linux applications.

---

# 14. Deadlock Detection

Sometimes,

systems allow deadlocks,

then detect them later.

Example

Database systems.

They periodically check

```
Who waits for whom?
```

If a cycle exists,

one transaction is rolled back.

Linux user-space programs generally try to **prevent** deadlocks rather than detect them automatically.

---

# 15. Debugging Deadlocks

Deadlocks can be difficult because the program appears to "freeze."

Useful tools:

### GDB

Attach to the process.

```
(gdb) thread apply all bt
```

This prints the backtrace of every thread.

You can see which mutex each thread is waiting on.

---

### Valgrind Helgrind

```
valgrind --tool=helgrind ./app
```

Helgrind detects many synchronization issues, including potential lock-order problems.

---

### ThreadSanitizer (TSan)

Compile with

```
-fsanitize=thread
```

TSan primarily detects data races, but it can also provide useful synchronization diagnostics.

---

### Logging

Log every lock and unlock.

Example

```text
Thread1 -> lock(m1)

Thread1 -> lock(m2)

Thread2 -> lock(m2)

Thread2 -> waiting for m1
```

This often reveals inconsistent lock ordering.

---

# 16. Common Mistakes

Mistake

```
Lock m1

↓

Lock m2
```

in one thread,

and

```
Lock m2

↓

Lock m1
```

in another.

This is the classic deadlock pattern.

---

Mistake

Holding locks while performing

```
sleep()

I/O

Network

File Access
```

This increases lock hold time and raises the risk of deadlock.

---

Mistake

Forgetting to unlock a mutex on an error path.

Example

```c
pthread_mutex_lock(&m);

if (error)
{
    return;
}
```

The mutex is never released.

A common solution is to structure cleanup carefully (for example, with a cleanup label in C).

---

# 17. Interview Questions

Q.

What is deadlock?

Answer

A situation where two or more threads are permanently blocked, each waiting for resources held by another.

---

Q.

What are the Coffman Conditions?

Answer

1. Mutual Exclusion

2. Hold and Wait

3. No Preemption

4. Circular Wait

All four must be present for a deadlock to occur.

---

Q.

How do you prevent deadlocks?

Answer

- Consistent lock ordering
- Keep critical sections short
- Avoid nested locks where possible
- Use `pthread_mutex_trylock()` when appropriate
- Design locking strategies carefully

---

Q.

Can semaphores also deadlock?

Yes.

Any synchronization mechanism can participate in a deadlock if threads wait on resources in a circular dependency.

---

Q.

Does increasing CPU cores solve deadlocks?

No.

Deadlock is a synchronization problem,

not a CPU performance problem.

---

# Summary

| Feature | Deadlock |
|----------|----------|
| Program Progress | Stops |
| CPU Usage | Often low (threads are blocked) |
| Incorrect Data | No |
| Root Cause | Circular resource dependency |
| Prevention | Lock ordering, short critical sections, careful design |
| Debug Tools | GDB, Helgrind, TSan, logging |

---

# Key Points to Remember

✓ Deadlock means **no thread can make progress**.

✓ It is different from a race condition.

✓ All four Coffman conditions are required.

✓ Lock ordering is one of the simplest and most effective prevention techniques.

✓ `pthread_mutex_trylock()` and `pthread_mutex_timedlock()` can be useful tools, but they are not universal solutions.

✓ Good synchronization design is easier than debugging a deadlock after deployment.

---

End of Chapter 7
