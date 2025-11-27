# process management
#### ✅ 1. Explain the concept of process creation in operating systems.

Process creation means creating a new process from an existing one.
In Linux:

A process creates a new process using fork()

The new process is called child

The original is called the parent

Both processes run independently

#### ✅ 2. Differentiate between fork() and exec() system calls.
| **fork()**                         | **exec()**                          |
| ---------------------------------- | ----------------------------------- |
| Creates a **new process (child)**  | Replaces the **current process**    |
| Child is a copy of parent          | Loads a **new program** into memory |
| Returns twice (to parent & child)  | Does NOT return if successful       |
| PID differs between parent & child | PID remains same                    |



Simple:
fork() = make a new process
exec() = run a new program inside the same process

#### ✅ 3. Write a C program to demonstrate the use of fork().
```
#include <stdio.h>
#include <unistd.h>

int main() {
    int pid = fork();

    if (pid == 0) {
        printf("This is child process\n");
    } else {
        printf("This is parent process\n");
    }
    return 0;
}
```
#### ✅ 4. What is the purpose of the wait() system call?

wait() is used by the parent process to:

Wait for the child process to finish

Prevent creation of zombie processes

Get the exit status of the child

#### ✅ 5. Describe the role of exec() family of functions.

exec() functions (execl, execv, execvp, etc.) are used to:

Replace the current running program

Load and execute a new program

Keep the same PID, but new code runs

Used in shells, system utilities, etc.

#### ✅ 6. Write a C program to illustrate execvp().
```
#include <stdio.h>
#include <unistd.h>

int main() {
    char *args[] = {"ls", "-l", NULL};

    execvp("ls", args);

    printf("This line will run only if execvp fails\n");
    return 0;
}
```
#### ✅ 7. How does vfork() differ from fork()?

| **fork()**                                         | **vfork()**                                              |
| -------------------------------------------------- | -------------------------------------------------------- |
| Creates a child with a **separate copy** of memory | Child **shares parent memory**                           |
| Slower                                             | Faster                                                   |
| Parent & child run independently                   | Parent is **blocked** until child calls exec() or exit() |
| Safe                                               | Dangerous (sharing memory)                               |

#### ✅ 8. Significance of getpid() and getppid().

getpid() → returns PID of current process

getppid() → returns parent’s PID

Useful for debugging and process management.

#### ✅ 9. Explain the concept of process termination in UNIX-like OS.

A process ends when:

It finishes executing main()

Calls exit()

Is killed by another process using kill()

Crashes (segmentation fault)

After termination:

Process becomes a zombie until parent calls wait()

Kernel frees resources

#### ✅ 10. Write a C program to create a child process using fork() and print its PID.
```
#include <stdio.h>
#include <unistd.h>
int main() {
    int pid = fork();

    if (pid == 0) {
        printf("Child Process PID: %d\n", getpid());
    } else {
        printf("Parent Process PID: %d\n", getpid());
    }

    return 0;
}
```

#### 11. Describe the process hierarchy in UNIX-like operating systems.

UNIX follows a tree-like process hierarchy:

The first process is init (or systemd), with PID 1.

All other processes are created from existing processes using fork().

Every process has:

A PID (process ID)

A PPID (parent process ID)

If a parent dies before its child, the child becomes an orphan and is adopted by PID 1.

#### 12. What is the purpose of the exit() function in C programming?

exit() is used to terminate a program immediately. It:

Cleans up resources (buffers, open files)

Returns an exit status to the parent process

Triggers termination handlers if registered

Example:
exit(0); → normal termination
exit(1); → abnormal termination

#### 13. Explain how the execve() system call works and provide a code example.

execve() replaces the current process image with a new program.

Steps:

Loads a new executable into memory

Removes old code, data, stack

Keeps the same PID

Starts executing the new program from main()

Example
```
#include <unistd.h>
#include <stdio.h>

int main() {
    char *args[] = {"/bin/echo", "Hello from execve", NULL};
    char *env[] = {NULL};

    execve("/bin/echo", args, env);

    perror("execve failed");
    return 1;
}
```
#### 14. Discuss the role of the fork() system call in implementing multitasking.

fork() enables multitasking by:

Creating multiple processes that run independently

Allowing concurrent execution of parent and child

Giving each process its own memory space

Operating systems then schedule these processes, switching between them rapidly, creating the illusion of parallel execution.

#### 15. C program to create multiple child processes and display their PIDs
```
#include <stdio.h>
#include <unistd.h>

int main() {
    for (int i = 0; i < 3; i++) {
        int pid = fork();

        if (pid == 0) {   // Child
            printf("Child %d PID: %d\n", i, getpid());
            return 0;     // Child exits so no further forks inside child
        }
    }
    return 0;
}
```

16. How does the exec() system call replace the current process image with a new one?

exec():

Removes (overwrites) current program’s memory

Loads a new executable file

Reinitializes stack, heap, text, and data sections

Starts the new program from main()

Never returns if successful

It keeps the same PID, but runs a completely new program.

#### 17. Explain the concept of process scheduling in operating systems.

Process scheduling is how the OS decides which process runs next.

Key points:

Uses scheduling algorithms (Round Robin, Priority, etc.)

Distributes CPU time among processes

Ensures fairness, responsiveness, and efficiency

Allows multitasking by switching between processes rapidly (context switching)

#### 18. Describe the role of the clone() system call in process management.

clone() is a low-level system call used to create:

Threads

Lightweight processes

It allows fine control over what is shared:

Memory

File descriptors

Signal handlers

Filesystem information

Used internally by pthread_create().

#### 19. Write a program in C to create a zombie process and explain how to avoid it.

Zombie process program

```
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int pid = fork();

    if (pid == 0) {
        printf("Child exiting...\n");
        exit(0);
    } else {
        printf("Parent sleeping, child becomes zombie...\n");
        sleep(10); // Child becomes zombie until parent waits
    }

    return 0;
}
```
How to avoid zombie processes

Parent must call wait() or waitpid() to clean up child process.

Example fix:
```
wait(NULL);
```
#### 20. Discuss the significance of the setuid() and setgid() system calls.

setuid(uid) and setgid(gid) are used to change the user ID and group ID of a process.

Purpose:

Implement privilege dropping (run as non-root)

Allow certain programs to run with elevated privileges (setuid binaries)

Control access to files and resources

Example:
A root program can switch to a normal user:
```
setuid(1000);
```

#### 21. Explain the concept of process groups and their significance in UNIX-like operating systems.

A process group is a collection of related processes.

Identified by a PGID

Used for job control (foreground/background jobs)

Signals can be sent to the whole group

Pipelines operate as a single process group

#### 22. Write a C program to demonstrate the use of waitpid() for process synchronization.
```
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pid = fork();

    if (pid == 0) {
        printf("Child running...\n");
        sleep(2);
        return 5;
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Child exited with status: %d\n", WEXITSTATUS(status));
    }
    return 0;
}
```
 #### 23. Discuss the role of the execle() function in the exec() family of calls.

execle():

Executes a new program

Allows passing a custom environment

Syntax:
```

execle(path, arg1, arg2, ..., NULL, envp);
```
#### 24. Describe the purpose of the nice() system call in process scheduling.

nice() adjusts process priority.

Range: −20 (high priority) → 19 (low priority)

Higher values = lower priority

Only root can use negative values
```
nice(10);
```

#### 25. Write a program in C to create a daemon process.
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    int pid = fork();
    if (pid > 0) exit(0);

    setsid();
    chdir("/");
    umask(0);

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    while (1) {
        sleep(10);
    }
    return 0;
}
```
#### 26. Explain the role of the getpid() and getppid() functions in process management.

getpid() → returns current process ID

getppid() → returns parent process ID

Used for monitoring and debugging process hierarchy.

#### 27. Discuss the difference between the fork() and clone() system calls.

fork()

Creates a new process with separate memory

Traditional UNIX process creation

clone()

Allows fine control over what is shared

Used internally to create threads

More flexible but lower-level

#### 28. Write a C program to demonstrate the use of the system() function for executing shell commands.
```
#include <stdlib.h>
#include <stdio.h>

int main() {
    printf("Executing shell command...\n");
    system("ls -l");
    return 0;
}
```
#### 29. Explain the concept of process states in UNIX-like operating systems.

Process states include:

R – Running

S – Sleeping

D – Uninterruptible sleep

Z – Zombie

T – Stopped

#### 30. Describe the purpose of the chroot() system call and provide an example.

chroot() changes the root directory of a process (creates a chroot jail).

```
#include <unistd.h>
#include <stdio.h>

int main() {
    if (chroot("/newroot") == 0) {
        chdir("/");
        printf("chroot successful.\n");
    } else {
        perror("chroot failed");
    }
    return 0;
}
```
