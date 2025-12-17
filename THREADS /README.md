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
