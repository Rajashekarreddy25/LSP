# LSP
#### 🔹 What is Process Synchronization?

In an operating system, multiple processes may run at the same time.

Sometimes, two or more processes need to access the same resource (like a file, variable, or printer).

If they try to use it at the same time, conflicts or incorrect results may occur → this is called a race condition.

Process Synchronization ensures that shared resources are used in a controlled way so that data consistency and correctness are maintained.

#### 🔹 Why is Synchronization Needed?

Avoid race conditions (two processes updating shared data at the same time).

Ensure data consistency (no wrong results).

Coordinate processes (e.g., producer must add data before consumer can read it).

#### 🔹 Critical Section Problem

Critical Section = part of code where a process accesses a shared resource.

Only one process at a time should execute in the critical section.

Solutions must follow:

Mutual Exclusion – only one process at a time in critical section.

Progress – if no process is inside, one should be allowed to enter.

Bounded Waiting – a process should not wait forever.

#### 🔹 Synchronization Techniques

1. Software Methods (Algorithms)

- Peterson’s Algorithm → works for 2 processes using flags and turn variables.

- Bakery Algorithm → like a token system (each process gets a number, smallest goes first).

2. Hardware Support

- Test-and-Set Instruction (TSL)

- Compare-and-Swap Instruction

- These are atomic (cannot be interrupted), used for locks.

#### Synchronization Tools in OS

- Semaphores: integer variable that controls access.

- Binary Semaphore → acts like a lock.

- Counting Semaphore → allows limited number of processes.

- Mutex (Mutual Exclusion Lock): only one process can hold the lock.

- Monitors: high-level construct (used in Java, etc.).

#### 🔹 Classic Problems in Synchronization

These are examples to understand synchronization:

- Producer-Consumer Problem

Producer puts data in buffer, consumer removes it.

Must ensure producer doesn’t add when buffer is full, and consumer doesn’t remove when empty.

- Dining Philosophers Problem

Multiple philosophers want to eat using limited forks → must prevent deadlock.

- Readers-Writers Problem

Multiple readers can read at the same time, but writers need exclusive access.

#### 🔹 Example with Semaphore (simple C-like pseudocode)
```
semaphore mutex = 1;

Process P1 {
    wait(mutex);     // lock
    // critical section
    signal(mutex);   // unlock
}

Process P2 {
    wait(mutex);
    // critical section
    signal(mutex);
}

```
👉 wait() decreases semaphore (lock it).

👉 signal() increases semaphore (unlock it).

👉 Ensures only one process enters critical section at a time.
