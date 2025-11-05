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



## 🧩 Program Code
```
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int x = 20;

int main()
{
    int stat, id;
    id = fork();

    if (id == 0) {
        x = 10;
        printf("child process\n");
        exit(0);
    }

    id = wait(&stat); // parent waits for child to finish
    printf("%d\n", WEXITSTATUS(stat));
    printf("%d\n", x);
    return 0;
}
```
### ⚙️ Step-by-Step Explanation
#### 1. Global variable

```
int x = 20;
```
This variable x is shared by both parent and child at the time of fork.

However, after fork(), each process has its own copy (Copy-On-Write mechanism).

#### 2. fork()
```
id = fork();
```

fork() creates a new process (child).

It returns:

0 → inside the child process.

child PID → inside the parent process.

So now there are two processes running concurrently.

#### 3. Child process block
```
if (id == 0) {
    x = 10;
    printf("child process\n");
    exit(0);
}
```

The child modifies its own copy of x to 10.

Then it prints "child process".

Then it terminates with exit(0) (exit status 0).

#### 4. Parent process
```
id = wait(&stat);
printf("%d\n", WEXITSTATUS(stat));
printf("%d\n", x);

```
1. wait(&stat) makes the parent wait for the child to finish.

2. After the child exits, stat contains the exit status.

3. WEXITSTATUS(stat) extracts that exit code (which is 0 here).

4. Then it prints x, which is still 20 in the parent (child’s change doesn’t affect parent’s variable).

##### 🧮 Output
child process
0
20

#### 🧠 Explanation of Output
Process	x value	Output Explanation

Child	10	Prints “child process” and exits with code 0

Parent	20	Waits for child, prints WEXITSTATUS=0 and x=20

#### ✅ Key Takeaways

After fork(), both parent and child have independent copies of data.

Changing a variable in the child does not affect the parent.

wait() ensures the parent waits for the child’s completion.

WEXITSTATUS(stat) retrieves the exit code passed by exit() or return.

## 🧩 Code: Demonstrating Copy-On-Write After fork()
```
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int x = 20;

int main() {
    int stat, id;
    printf("Parent process started (PID: %d)\n", getpid());
    printf("Address of x in parent: %p, Value of x: %d\n\n", (void*)&x, x);

    id = fork();

    if (id == 0) {  // Child process
        printf("Child process (PID: %d, PPID: %d)\n", getpid(), getppid());
        printf("Address of x in child: %p, Value of x before change: %d\n", (void*)&x, x);
        x = 10;
        printf("Child modified x to %d\n", x);
        printf("Address of x in child (after change): %p\n\n", (void*)&x);
        exit(0);
    }

    // Parent process
    wait(&stat);
    printf("Parent process (PID: %d) resumed after child exit.\n", getpid());
    printf("Address of x in parent: %p, Value of x: %d\n", (void*)&x, x);
    printf("Child exit status: %d\n", WEXITSTATUS(stat));

    return 0;
}
```
### 🧠 Expected Output Example
Parent process started (PID: 694)

Address of x in parent: 0x5c3cdb41c010, Value of x: 20

Child process (PID: 695, PPID: 694)

Address of x in child: 0x5c3cdb41c010, Value of x before change: 20

Child modified x to 10

Address of x in child (after change): 0x5c3cdb41c010

Parent process (PID: 694) resumed after child exit.

Address of x in parent: 0x5c3cdb41c010, Value of x: 20

Child exit status: 0

### 🔍 Explanation

Both parent and child initially show the same memory address for x.

→ This is because at the time of fork(), they share the same physical pages (due to Copy-On-Write).

When the child modifies x, the OS creates a new physical copy of the page for the child.

→ That’s why even though the virtual addresses are identical, they point to different physical memory.

The parent’s value remains unchanged (still 20), proving that each process has its own independent copy after modification.

## 🧠 What is Copy-On-Write (COW)?

- Copy-On-Write (COW) is an optimization technique used in operating systems during process creation (like fork()).

### 🔹 Basic idea:

When a process calls fork(), the child process needs to have the same memory image as the parent.

### 👉 But copying the entire memory (code, data, stack) immediately would be slow and wasteful — especially if the child process will soon call exec() to load a new program.

So, instead of copying everything right away, the OS does this smart thing:

Both parent and child share the same physical memory pages initially (read-only).

When either process tries to modify a page, the OS creates a private copy of that page for that process.

That’s why it’s called Copy-On-Write —

the copy happens only when a write occurs.

### ⚙️ How it works in steps

Step	Action	What Happens
- 1️⃣	Parent process calls fork()	OS creates a child process with the same virtual address space mapping as the parent.
- 2️⃣	Both share same physical pages	These pages are marked read-only to prevent modification.
- 3️⃣	If parent or child modifies a variable	The OS copies that memory page and gives a private writable copy to the modifying process.
- 4️⃣	The other process keeps its old page	So their changes do not interfere with each other.

### 🧩 Example (from your program)
Before modification:

Both processes have:

x = 20
(same virtual address, same physical memory)

After child modifies x:

Child writes x = 10

OS detects write attempt on shared page → creates child’s own copy

Parent still sees x = 20, child sees x = 10

So even though they print the same address for x, those addresses point to different physical memory after the write.

### 🧮 Analogy

Imagine two people (Parent and Child) sharing a Google Doc in read-only mode.
Both see the same content.

When the child wants to edit, the system makes a separate copy of the document for the child.
Now:

Parent’s document remains unchanged.

Child edits their own copy.

That’s exactly how Copy-On-Write works in memory.

#### ✅ Benefits of Copy-On-Write

Saves memory — no need to duplicate pages until necessary.

Improves performance — fork() becomes very fast.

Efficient with exec() — often, child replaces memory soon after fork.

Automatic isolation — processes don’t interfere with each other’s data.
