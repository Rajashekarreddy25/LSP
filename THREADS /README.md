### Deadlock
-   A deadlock occurs when a set of processes or threads are permanently blocked, each waiting for a resource held by another.

Deadlock happens only if all the following four conditions occur at the same time (called Coffman’s conditions):
### Causes 
1️. Mutual Exclusion

At least one resource must be non-shareable.

Only one process/thread can use the resource at a time.

📌 Example:
A mutex lock protecting a shared variable.

2️. Hold and Wait

A process holds at least one resource and is waiting for additional resources that are currently held by others.

📌 Example:
Thread A holds Lock1 and waits for Lock2
Thread B holds Lock2 and waits for Lock1

3️. No Preemption

Resources cannot be forcibly taken away from a process.

They must be released voluntarily after use.

📌 Example:
Once a thread acquires a mutex, the OS cannot take it back until the thread releases it.

4️. Circular Wait

A circular chain of processes exists where each process waits for a resource held by the next process in the chain.

📌 Example:
Thread A → waiting for Thread B
Thread B → waiting for Thread A

🔄 Simple Deadlock Example (Threads)
Thread A:               Thread B:
lock(mutex1);           lock(mutex2);
lock(mutex2);           lock(mutex1);


👉 Both threads wait forever → deadlock
