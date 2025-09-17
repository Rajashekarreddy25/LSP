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



### 🔹 1. Orphan Process

When a parent process terminates but its child is still running, the child becomes an orphan.

In Linux, orphan processes are adopted by init (PID 1 or systemd), so they don’t remain without a parent.

init takes responsibility to clean them up when they finish.

👉 Example
```
#include <stdio.h>
#include <unistd.h>

int main() {
    int pid = fork();

    if (pid > 0) {
        printf("Parent exiting... PID=%d\n", getpid());
        return 0;  // Parent ends
    }
    else if (pid == 0) {
        sleep(5);  // Child runs after parent exits
        printf("Child running... Orphan now, adopted by init. PID=%d, Parent PID=%d\n",
                getpid(), getppid());
    }
}
```

✅ Here, the child becomes an orphan because the parent exits before the child.

### 🔹 2. Zombie Process

When a child finishes execution but the parent hasn’t collected its exit status using wait() or waitpid(), it becomes a zombie.

Zombie = “dead but still in process table” (has PID entry but no resources).

Too many zombies can waste PIDs.

👉 Example
```
#include <stdio.h>
#include <unistd.h>

int main() {
    int pid = fork();

    if (pid == 0) {
        printf("Child finished. PID=%d\n", getpid());
        return 0;  // Child exits
    }
    else {
        sleep(20);  // Parent doesn't call wait(), child becomes zombie
        printf("Parent exiting...\n");
    }
}

```
✅ Run ps -l while parent is sleeping, you’ll see the child in Z (zombie) state.

### 🔹 3. Daemon Process

A daemon is a process that runs in the background, usually providing a service (like sshd, cron, httpd).

They are independent of any terminal and usually started at boot time.

Typically created by double fork technique:

Parent forks → child continues.

Child creates a new session (setsid()) → detaches from terminal.

Child forks again → grandchild runs as a daemon, no controlling terminal.

👉 Example Skeleton (Daemon creation)
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) exit(1);
    if (pid > 0) exit(0);   // Parent exits

    // Child becomes session leader
    if (setsid() < 0) exit(1);

    pid = fork();           // Second fork
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);   // First child exits

    // Daemon process runs here
    while (1) {
        sleep(10);
        // background task
    }
}
```

✅ This process will run in the background as a daemon.

🔹 Summary (Tabular Form)

| Process Type | How it Happens                                     | Who Adopts/Handles It                     | Example                         |
| ------------ | -------------------------------------------------- | ----------------------------------------- | ------------------------------- |
| **Orphan**   | Parent exits before child                          | `init` (PID 1) adopts it                  | Child sleeps while parent exits |
| **Zombie**   | Child exits but parent doesn’t `wait()`            | Stays in process table until parent waits | Parent sleeps, child exits      |
| **Daemon**   | Special background process, detached from terminal | Runs independently                        | `cron`, `sshd`                  |

👉 So:

Orphan = Parent gone

Zombie = Child gone but parent lazy

Daemon = Background worker
