# Linux System Programming Synchronization Guide

# 1. Mutex

## Purpose
A mutex (Mutual Exclusion) protects shared data so that **only one thread** accesses a critical section at a time.

### Important APIs

- `pthread_mutex_init(&mutex, NULL)` – initialize.
- `pthread_mutex_lock(&mutex)` – acquire the lock (thread sleeps if unavailable).
- `pthread_mutex_unlock(&mutex)` – release the lock.
- `pthread_mutex_destroy(&mutex)` – destroy the mutex.

### Important arguments
- First argument: pointer to the mutex.
- Second argument of `pthread_mutex_init`:
  - `NULL` → default attributes.

### Example

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t mutex;

void *worker(void *arg)
{
    for(int i=0;i<5;i++)
    {
        pthread_mutex_lock(&mutex);

        counter++;
        printf("Counter = %d\n", counter);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t t1,t2;

    pthread_mutex_init(&mutex,NULL);

    pthread_create(&t1,NULL,worker,NULL);
    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    pthread_mutex_destroy(&mutex);
}
```

---

# 2. Semaphore

## Purpose

A semaphore controls **how many threads** may access a resource simultaneously.

### Important APIs

- `sem_init(&sem,0,value)`
- `sem_wait(&sem)`
- `sem_post(&sem)`
- `sem_destroy(&sem)`

### Important arguments

`sem_init(&sem,0,2)`

- `&sem` → semaphore object
- `0` → shared between threads of same process
- `2` → initial count

### Example

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;

void *worker(void *arg)
{
    sem_wait(&sem);

    printf("Thread %ld entered\n",(long)arg);
    sleep(2);

    printf("Thread %ld leaving\n",(long)arg);

    sem_post(&sem);

    return NULL;
}

int main()
{
    pthread_t t[4];

    sem_init(&sem,0,2);

    for(long i=0;i<4;i++)
        pthread_create(&t[i],NULL,worker,(void*)i);

    for(int i=0;i<4;i++)
        pthread_join(t[i],NULL);

    sem_destroy(&sem);
}
```

---

# 3. Condition Variable

## Purpose

A condition variable allows a thread to **sleep until a condition becomes true**.

Always use it together with a mutex.

### Important APIs

- `pthread_cond_init()`
- `pthread_cond_wait()`
- `pthread_cond_signal()`
- `pthread_cond_broadcast()`
- `pthread_cond_destroy()`

### Important arguments

`pthread_cond_wait(&cond,&mutex)`

- `&cond` → condition variable
- `&mutex` → mutex protecting the shared state

`pthread_cond_wait()`:
1. Unlocks the mutex.
2. Puts the thread to sleep.
3. Re-locks the mutex before returning.

### Producer-Consumer Example (buffer size = 1)

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int buffer;
int full=0;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *producer(void *arg)
{
    for(int i=1;i<=10;i++)
    {
        pthread_mutex_lock(&mutex);

        while(full)
            pthread_cond_wait(&cond,&mutex);

        buffer=i;
        full=1;

        printf("Produced %d\n",buffer);

        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg)
{
    for(int i=1;i<=10;i++)
    {
        pthread_mutex_lock(&mutex);

        while(!full)
            pthread_cond_wait(&cond,&mutex);

        printf("Consumed %d\n",buffer);

        full=0;

        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t p,c;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);

    pthread_create(&p,NULL,producer,NULL);
    pthread_create(&c,NULL,consumer,NULL);

    pthread_join(p,NULL);
    pthread_join(c,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
```

---

# 4. Spinlock

## Purpose

A spinlock protects a very short critical section.

Unlike a mutex, a waiting thread **does not sleep**; it repeatedly checks until the lock becomes free.

### Important APIs

- `pthread_spin_init()`
- `pthread_spin_lock()`
- `pthread_spin_unlock()`
- `pthread_spin_destroy()`

### Important arguments

`pthread_spin_init(&lock,PTHREAD_PROCESS_PRIVATE)`

- `&lock` → spinlock object
- `PTHREAD_PROCESS_PRIVATE` → usable by threads in same process

### Example

```c
#include <stdio.h>
#include <pthread.h>

pthread_spinlock_t lock;
int counter=0;

void *worker(void *arg)
{
    for(int i=0;i<5;i++)
    {
        pthread_spin_lock(&lock);

        counter++;
        printf("%d\n",counter);

        pthread_spin_unlock(&lock);
    }

    return NULL;
}

int main()
{
    pthread_t t1,t2;

    pthread_spin_init(&lock,PTHREAD_PROCESS_PRIVATE);

    pthread_create(&t1,NULL,worker,NULL);
    pthread_create(&t2,NULL,worker,NULL);

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    pthread_spin_destroy(&lock);
}
```

---

# Main Differences

| Feature | Mutex | Semaphore | Condition Variable | Spinlock |
|--------|--------|-----------|--------------------|-----------|
| Purpose | Protect shared data | Count resources | Wait for state change | Short critical section |
| Owners | One | N | Not a lock | One |
| Sleeps while waiting | Yes | Yes | Yes | No |
| Busy waiting | No | No | No | Yes |
| CPU usage | Low | Low | Low | High |
| Used with mutex | No | No | Yes | No |
| Typical use | Shared variables | Resource pools | Producer-consumer | Kernel/very short waits |

# Interview Tips

- Use **Mutex** for protecting shared variables.
- Use **Semaphore** to limit concurrent access or for signaling.
- Use **Condition Variables** when waiting for a condition (queue not empty, work available).
- Use **Spinlocks** only when waiting is expected to be extremely short and sleeping is undesirable.
