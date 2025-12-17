### Deadlock
-   A deadlock occurs when a set of processes or threads are permanently blocked, each waiting for a resource held by another.

Deadlock happens only if all the following four conditions occur at the same time (called Coffman’s conditions):
### Causes 
**1️.** Mutual Exclusion

At least one resource must be non-shareable.

Only one process/thread can use the resource at a time.

📌 Example:
A mutex lock protecting a shared variable.

**2️.** Hold and Wait

A process holds at least one resource and is waiting for additional resources that are currently held by others.

📌 Example:
Thread A holds Lock1 and waits for Lock2
Thread B holds Lock2 and waits for Lock1

**3️.** No Preemption

Resources cannot be forcibly taken away from a process.

They must be released voluntarily after use.

📌 Example:
Once a thread acquires a mutex, the OS cannot take it back until the thread releases it.

**4️.** Circular Wait

A circular chain of processes exists where each process waits for a resource held by the next process in the chain.

📌 Example:
Thread A → waiting for Thread B
Thread B → waiting for Thread A

🔄 Simple Deadlock Example (Threads)
Thread A:               Thread B:
lock(mutex1);           lock(mutex2);
lock(mutex2);           lock(mutex1);


👉 Both threads wait forever → deadlock




 ## pthread_create() — Arguments Explained
### 🔹 Function Prototype
```
int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg
);
```

- It has 4 arguments.

**1️.** pthread_t *thread → Thread ID (Output)
What it is:

- Pointer to a variable that will store the thread identifier

Example:
```
pthread_t tid;
pthread_create(&tid, ...);
```
Why needed:

- Used later with:

  - pthread_join()

  - pthread_cancel()

  - pthread_detach()

- OS assigns the thread ID, you just store it.

**2️.** const pthread_attr_t *attr → Thread Attributes
What it is:

- Controls thread behavior (stack size, scheduling, detached state)

Common usage:
```
NULL
```

- ➡ Uses default attributes

Advanced usage:
```
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_create(&tid, &attr, func, arg);
```

- For most programs & exams → use NULL

**3️.** void *(*start_routine)(void *) → Thread Function
What it is:

- Function executed by the thread

- This is where the thread starts running

Required function signature:
```
void *thread_function(void *arg);
```
Example:
```
void *worker(void *arg)
{
    printf("Thread running\n");
    return NULL;
}
```

- Mandatory argument
- Thread always starts execution here

**4️.** void *arg → Argument to Thread Function
What it is:

Data passed to the thread function

Generic pointer (void *)

Example (integer):
```
int x = 10;
pthread_create(&tid, NULL, worker, &x);
```

Inside thread:
```
int n = *(int *)arg;
```
- If no argument needed:
```
pthread_create(&tid, NULL, worker, NULL);
```
### Complete Example With All Arguments
```
#include <stdio.h>
#include <pthread.h>

void *thread_func(void *arg)
{
    int n = *(int *)arg;
    printf("Value = %d\n", n);
    return NULL;
}

int main()
{
    pthread_t tid;
    int x = 5;

    pthread_create(&tid, NULL, thread_func, &x);
    pthread_join(tid, NULL);

    return 0;
}
```
### Summary Table (Very Important)
| Argument        | Meaning                            |
| --------------- | ---------------------------------- |
| `&thread`       | Stores thread ID                   |
| `attr`          | Thread attributes (usually `NULL`) |
| `start_routine` | Function executed by thread        |
| `arg`           | Argument passed to function        |



## 79. Why do we use pthread_create() instead of clone() for creating threads?

- clone() is a low-level Linux system call

- pthread_create() is a POSIX standard, portable, high-level API

**Reasons:**

**- pthread_create():**

   - Portable across UNIX/Linux systems
   
   - Automatically handles stack, TLS, scheduling
   
   - Easier and safer to use

**- clone():**

  - OS-specific
  
  - Requires manual stack and flags management
  
  - Error-prone

✅ Conclusion:
- pthread_create() internally uses clone() but provides abstraction and safety.

## 80. Why does a stack grow?

**Stack grows to:**

- Store function call information

- Local variables

- Function parameters

- Return addresses

**Direction:**

- Stack typically grows downwards (high → low memory)

**Example:**
```
void func() {
   int x;   // stored on stack
}
```
## 81. What segments are shared by multiple threads within a process?

**- Threads share:**

  - Code (Text) segment
  
  - Data segment
  
  - BSS segment
  
  - Heap
  
  - File descriptors
  
  - Global variables

**- Each thread has:**

  - Its own stack
  
  - Its own registers
  
  - Its own thread ID

## 82. Can you fetch the thread entry point return value in the main thread?

**- Yes**

Use **pthread_join():**
```
void *ret;
pthread_join(thread, &ret);
```

- The value returned from the thread function is received by the main thread

## 83. What happens when main() function is invoked?

**Steps:**

1. Program loaded into memory

2. Stack, heap, data segments initialized

3. Runtime calls main()

4. Execution begins from main()

main() is not the first instruction,**_start() **is.

## 84. What happens when CPU stops executing?

CPU may stop due to:

  - Context switch
  
  - Interrupt
  
  - System call
  
  - Halt instruction
  
  - Power management

OS saves:

  - Program Counter
  
  - Registers
  
  - Stack pointer

Into PCB (Process Control Block)

## 85. During context switch, which instruction copies CPU registers to PCB?

- There is no single instruction.

OS kernel code saves registers using:

  - Assembly instructions
  
  - Stack push operations
  
  - Architecture-specific save routines

**Example (conceptual):**
```
push eax
push ebx
```
## 86. How do you create a separate process?

**Using:
**

- fork()

- exec()

**Example:
**
```
pid_t pid = fork();
```

Parent → original process

Child → new process

## 87. How does a server create a separate thread?

**Server uses:**

- pthread_create()


**For:**

- Each client request

- Parallel request handling

**Example:**
```
pthread_create(&tid, NULL, handle_client, arg);
```
## 88. Advantage of Thread over Process

| Threads         | Processes       |
| --------------- | --------------- |
| Faster creation | Slower creation |
| Shared memory   | Separate memory |
| Low overhead    | High overhead   |
| Efficient IPC   | Costly IPC      |


✅ Best for parallel tasks

## 89. Advantage of Process over Thread

- Better isolation

- One process crash won’t affect others

- Higher security

- Separate address space

✅ Best for fault isolation

## 90. How to overcome synchronization issues with global variables?

Use:

 - Mutex

 - Semaphore

 - Spinlock

 - Atomic operations

**Example:**
```
pthread_mutex_lock(&lock);
global++;
pthread_mutex_unlock(&lock);
```
## 91. How much CPU time is given to user and kernel threads?

- CPU time is not fixed

- Allocated by scheduler

- Depends on:

  - Priority

  - Scheduling policy

 User and kernel threads get equal scheduling opportunity

## 92. Difference between POSIX and System-V
| POSIX       | System-V     |
| ----------- | ------------ |
| Portable    | Older UNIX   |
| pthreads    | Sys-V IPC    |
| Simple APIs | Complex APIs |
| Modern      | Legacy       |

## 93. Points to remember when using mutex locks

- Lock before critical section

- Unlock after critical section

- Avoid deadlock

- Same thread must unlock

- Always initialize mutex

## 94. What do you achieve using pthread_mutex_lock()?

- Mutual exclusion

- Prevent race condition

- Ensure one thread accesses critical section at a time

## 95. Difference between Mutex and Semaphore

| Mutex              | Semaphore               |
| ------------------ | ----------------------- |
| Binary             | Counting                |
| Owner-based        | No ownership            |
| One thread unlocks | Any thread can signal   |
| Used for CS        | Used for resource count |


## 97. Variants of pthread_mutex_lock()

- pthread_mutex_lock() – blocks

- pthread_mutex_trylock() – non-blocking

- pthread_mutex_timedlock() – timeout based

## 97. How to create a thread?
```
pthread_create(&tid, NULL, func, arg);

```
**Steps:**

1. Declare pthread_t

2. Call pthread_create()

3. Join or detach

## 98. Explain compilation of a thread program
```
gcc file.c -o output -pthread

```

- pthread enables:

 - Thread-safe libc

 - Proper linking

## 99. Arguments of pthread_create()
```
int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg
);
```
## 100. Explain return value of thread

- Thread returns:
```
return value;
```

- Collected using:
```
pthread_join(thread, &ret);
```
## 101. Working of pthread_mutex_trylock()

- Tries to lock mutex

- If already locked → returns immediately

- Does NOT block

- Use case: avoid waiting

## 102. Application of pthread_mutex_timedlock()

- Locks mutex with timeout

- Avoids deadlock

- Used in real-time systems

## 103. What is Mutual Exclusion?

- Technique to ensure:

  - Only one thread enters critical section

- Achieved using:

   - Mutex
   
   - Semaphore

 **Example:** 

pthread_mutex_lock(&lock);
// critical section
pthread_mutex_unlock(&lock);
