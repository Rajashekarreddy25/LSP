## Condition Variables in Pthreads 

##### 1. What Problem Do Condition Variables Solve?

- A mutex ensures that only one thread accesses a shared variable at a time.
- However, a mutex cannot make a thread wait efficiently for a change in the state of a shared variable.
- Without condition variables (busy waiting)
  ```
while (avail == 0) {
    // keep checking
}
```

##### Problems:

Wastes CPU time

Thread runs continuously

Poor performance and scalability

###### Solution

➡️ Condition variables allow a thread to:

- Sleep until something changes

- Be woken up by another thread when work is available

#### 2. What Is a Condition Variable?

- A condition variable is a synchronization object used to:

- Notify one or more waiting threads that a shared variable’s state has changed

##### Important points:

- A condition variable does not store data

- It only communicates that “something may have changed”

- It must always be used together with a mutex

#### 3. Relationship Between Mutex and Condition Variable
- Object	Purpose
- Mutex	Protects shared data
- Condition Variable	Signals change in shared data

**Key rule:**

- A condition variable is meaningless without a mutex.

#### 4. Declaring and Initializing Condition Variables
**4.1 Static Initialization**
```
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

- Used for global or static variables

- No need to destroy explicitly

**4.2 Important SUSv3 Rule**

- Using a copy of a condition variable results in undefined behavior.

✔ Always operate on the original condition variable.

#### 5. Core Condition Variable Functions
**5.1 Signaling Functions**
```
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```
**Function	Behavior**
- signal	Wakes one waiting thread
- broadcast	Wakes all waiting threads

**Both return:**
```
0 on success

Positive error number on failure
```
**5.2 Waiting Function**
```
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```

**Purpose:**

- Blocks the calling thread until cond is signaled

#### 6. Difference Between signal() and broadcast()

**pthread_cond_signal()**

- Wakes at least one waiting thread

- More efficient

**Use when:
**
- All waiting threads perform the same task

- Only one thread needs to handle the change

**pthread_cond_broadcast()**

- Wakes all waiting threads

**Necessary when:**

- Threads perform different tasks

- Multiple predicates exist

#### 7. Why pthread_cond_signal() Is More Efficient

**If broadcast() is used:**

- All threads wake up

- One thread does the work

**Remaining threads:
**
- Lock mutex

- Find no work

- Go back to sleep

**This causes:**

- Extra context switches

- Performance loss

#### 8. Condition Variables Have No Memory

**Important property:
**
- If no thread is waiting when a condition variable is signaled, the signal is lost.

**Therefore:**

- Condition variables do not remember signals

- Shared variables store actual state

#### 9. How pthread_cond_wait() Works Internally

1. Unlock the mutex
2. Block the thread
3. Wait for signal
4. Re-lock the mutex


✔ Steps 1 and 2 are atomic
✔ No race condition is possible

#### 10. Natural Pattern for Using Condition Variables
```
pthread_mutex_lock(&mtx);

while (predicate_not_true) {
    pthread_cond_wait(&cond, &mtx);
}

/* Shared state is now valid */

pthread_mutex_unlock(&mtx);
```
#### 11. Why while Is Mandatory (NOT if)

- A thread must re-check the predicate after waking up.

**Reasons:**

- Another thread may run first and change the state

- Condition variables allow spurious wakeups

- Multiple threads may wake up

**Correct:**
```
while (avail == 0)
    pthread_cond_wait(&cond, &mtx);
```

Incorrect:
```
if (avail == 0)
    pthread_cond_wait(&cond, &mtx);
```
#### 12. Spurious Wakeups

**POSIX allows a thread to wake up:
**
-  Without any signal

- Due to internal OS optimizations

**Therefore:**

- A wakeup does NOT guarantee the condition is true.

#### 13. Example: Producer–Consumer Using Condition Variables
**Shared Objects**
```
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int avail = 0;
```
**Producer Thread**
```
pthread_mutex_lock(&mtx);
avail++;                     // produce item
pthread_mutex_unlock(&mtx);

pthread_cond_signal(&cond);  // notify consumer
```
**Consumer Thread**
```
pthread_mutex_lock(&mtx);

while (avail == 0) {
    pthread_cond_wait(&cond, &mtx);
}

avail--;  // consume item

pthread_mutex_unlock(&mtx);
```
#### 14. Order of signal() and unlock()

**Allowed orders:**
```
pthread_mutex_unlock(&mtx);
pthread_cond_signal(&cond);
```

or
```
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mtx);
```
- Performance Note (Butenhof)

- Unlocking before signaling may avoid extra context switches

- Some systems optimize this using wait morphing

#### 15. Timed Waiting on Condition Variables
```
int pthread_cond_timedwait(
    pthread_cond_t *cond,
    pthread_mutex_t *mutex,
    const struct timespec *abstime
);

```
**Blocks until:
**
- Condition is signaled OR

- Timeout expires

- Returns ETIMEDOUT on timeout

#### 16. Dynamically Allocated Condition Variables
**Initialization**
```
pthread_cond_init(&cond, NULL);

```
**Required for:**

- Stack-allocated variables

- Dynamically allocated memory

- Non-default attributes

**Destruction**
```
pthread_cond_destroy(&cond);
```

**Rules:**

- No threads must be waiting

- Destroy before freeing memory

- Can reinitialize later

#### 17. Condition Variable Predicate

- Each condition variable has an associated predicate.

**Example:**
```
predicate: (avail > 0)
```

**Condition variable means:**

- “The predicate may now be true”
