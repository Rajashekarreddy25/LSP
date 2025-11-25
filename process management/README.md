# process management
##### ✅ 1. Explain the concept of process creation in operating systems.

Process creation means creating a new process from an existing one.
In Linux:

A process creates a new process using fork()

The new process is called child

The original is called the parent

Both processes run independently

##### ✅ 2. Differentiate between fork() and exec() system calls.
| **fork()**                         | **exec()**                          |
| ---------------------------------- | ----------------------------------- |
| Creates a **new process (child)**  | Replaces the **current process**    |
| Child is a copy of parent          | Loads a **new program** into memory |
| Returns twice (to parent & child)  | Does NOT return if successful       |
| PID differs between parent & child | PID remains same                    |



Simple:
fork() = make a new process
exec() = run a new program inside the same process

##### ✅ 3. Write a C program to demonstrate the use of fork().
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
##### ✅ 4. What is the purpose of the wait() system call?

wait() is used by the parent process to:

Wait for the child process to finish

Prevent creation of zombie processes

Get the exit status of the child

##### ✅ 5. Describe the role of exec() family of functions.

exec() functions (execl, execv, execvp, etc.) are used to:

Replace the current running program

Load and execute a new program

Keep the same PID, but new code runs

Used in shells, system utilities, etc.

##### ✅ 6. Write a C program to illustrate execvp().
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
##### ✅ 7. How does vfork() differ from fork()?

| **fork()**                                         | **vfork()**                                              |
| -------------------------------------------------- | -------------------------------------------------------- |
| Creates a child with a **separate copy** of memory | Child **shares parent memory**                           |
| Slower                                             | Faster                                                   |
| Parent & child run independently                   | Parent is **blocked** until child calls exec() or exit() |
| Safe                                               | Dangerous (sharing memory)                               |

##### ✅ 8. Significance of getpid() and getppid().

getpid() → returns PID of current process

getppid() → returns parent’s PID

Useful for debugging and process management.

##### ✅ 9. Explain the concept of process termination in UNIX-like OS.

A process ends when:

It finishes executing main()

Calls exit()

Is killed by another process using kill()

Crashes (segmentation fault)

After termination:

Process becomes a zombie until parent calls wait()

Kernel frees resources

#####✅ 10. Write a C program to create a child process using fork() and print its PID.
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
