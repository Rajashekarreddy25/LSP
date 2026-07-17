# Linux System Programming Handbook

# Chapter 4
# Condition Variables (pthread_cond)

---

# Table of Contents

1. Why Mutex is NOT Enough
2. The Real Problem
3. What is a Condition Variable?
4. Real-Life Analogy
5. Internal Working
6. Why pthread_cond_wait() Needs a Mutex
7. Condition Variable APIs
8. Step-by-Step Execution
9. Lost Wake-up Problem
10. Why while() and NOT if()
11. signal() vs broadcast()
12. Producer-Consumer Example (Buffer Size = 1)
13. Producer Produces Continuously & Consumer Consumes 10 Items
14. Common Mistakes
15. Real World Use Cases
16. Interview Questions
17. Summary

---

# 1. Why Mutex is NOT Enough?

Most beginners think

```
Mutex

↓

Problem Solved
```

This is only partially true.

A mutex protects

```
Shared Data
```

It DOES NOT tell another thread

```
When

data

becomes

available.
```

This is the biggest misunderstanding.

---

Example

Producer Thread

```
Produces Data
```

Consumer Thread

```
Needs Data
```

Suppose

Initially

```
Queue Empty
```

Consumer

```
Locks Mutex

↓

Checks Queue

↓

Queue Empty
```

Now what?

Consumer has two options.

Option 1

```
Keep checking

again

and

again

and

again
```

CPU becomes

```
100%
```

Busy Waiting.

Very bad.

---

Option 2

Consumer sleeps.

But

```
How

does

it

know

when

producer

creates

data?
```

Mutex cannot answer this.

Condition Variable solves exactly this problem.

---

# 2. The Real Problem

Suppose

```
Queue

↓

Empty
```

Consumer

```
while(queue empty)

{

?????

}
```

Without condition variable

Developer writes

```c
while(queue_empty)
{
    sleep(1);
}
```

This works...

BUT

Very inefficient.

---

Timeline

```
Producer

Produces after

5 milliseconds
```

Consumer

```
Sleeping

for

1 second
```

Producer finished long ago.

Consumer still sleeping.

Huge delay.

---

Another approach

```c
while(queue_empty)
{
}
```

Infinite loop.

CPU

```
100%
```

This is called

```
Busy Waiting
```

---

Condition Variable provides

```
Sleep

WITHOUT

wasting CPU
```

AND

Wake immediately

when condition changes.

---

# 3. What is a Condition Variable?

Definition

A condition variable allows a thread to

```
Sleep

until

a particular

condition

becomes

TRUE.
```

Examples

```
Queue Not Empty

Buffer Not Full

Download Completed

Sensor Ready

Task Available

Frame Captured

Socket Connected
```

Notice

Condition Variable

does NOT protect data.

Mutex protects data.

Condition Variable

only waits for

```
State Change
```

---

# 4. Real Life Analogy

Restaurant

Customer

```
Food Ready?
```

Chef

```
No
```

Customer

```
Keeps asking

every second?
```

Bad.

Instead

Chef says

```
Sit.

I'll call you.
```

Customer sleeps.

Chef finishes cooking.

Chef calls customer.

Customer wakes immediately.

Exactly how

Condition Variable works.

---

# 5. Internal Working

Conceptually

Condition Variable contains

```
Wait Queue
```

Initially

```
Wait Queue

↓

Empty
```

Consumer

calls

```
pthread_cond_wait()
```

Now

```
Wait Queue

↓

Consumer Thread
```

Producer

calls

```
pthread_cond_signal()
```

Kernel

```
Removes

one thread

↓

Ready Queue
```

Consumer resumes execution.

---

# 6. Why pthread_cond_wait() Needs a Mutex

This is probably the MOST ASKED interview question.

Function

```c
pthread_cond_wait(
        &cond,
        &mutex);
```

People ask

```
Why

two arguments?
```

Let's understand.

Suppose

Consumer

```
Locks Mutex

↓

Queue Empty

↓

Needs to Sleep
```

Question

If Consumer sleeps

WITHOUT

unlocking mutex

What happens?

Producer

tries

```
Lock Mutex
```

Blocked.

Producer cannot produce.

Consumer waiting.

Producer waiting.

Deadlock.

---

Therefore

pthread_cond_wait()

does

THREE THINGS

automatically.

```
Unlock Mutex

↓

Sleep

↓

Wake Up

↓

Lock Mutex Again

↓

Return
```

This is the most important behavior.

---

Timeline

Consumer

```
Lock Mutex

↓

Queue Empty

↓

pthread_cond_wait()
```

Internally

```
Unlock Mutex

↓

Sleep
```

Producer

```
Lock Mutex

↓

Produce

↓

Signal

↓

Unlock
```

Consumer

```
Wake

↓

Automatically Lock Mutex

↓

Continue
```

---

# 7. Condition Variable APIs

## pthread_cond_init()

Initializes condition variable.

```c
pthread_cond_init(
        &cond,
        NULL);
```

---

## pthread_cond_wait()

```c
pthread_cond_wait(
        &cond,
        &mutex);
```

Internally

```
Unlock Mutex

↓

Sleep

↓

Wake

↓

Lock Mutex

↓

Return
```

---

## pthread_cond_signal()

Wakes

```
ONE

waiting thread.
```

---

## pthread_cond_broadcast()

Wakes

```
ALL

waiting threads.
```

---

## pthread_cond_destroy()

Destroys condition variable.

---

# 8. Step-by-Step Execution

Producer

Consumer

Initially

```
Queue Empty
```

Consumer

```
Lock Mutex

↓

Queue Empty

↓

pthread_cond_wait()
```

Consumer sleeps.

Producer

```
Lock Mutex

↓

Insert Data

↓

Signal

↓

Unlock
```

Consumer wakes.

Automatically locks mutex.

Consumes data.

Unlocks mutex.

---

# 9. Lost Wake-up Problem

Suppose

Producer

signals

BEFORE

Consumer starts waiting.

Timeline

```
Producer

Signal

↓

Consumer

Wait
```

Signal already lost.

Consumer waits forever.

This is called

```
Lost Wake-up
```

Solution

Always check

condition

inside

```
while()
```

---

# 10. Why while() Instead of if()

Wrong

```c
if(queue_empty)
    pthread_cond_wait();
```

Correct

```c
while(queue_empty)
    pthread_cond_wait();
```

Why?

Because

A thread may wake

without

condition becoming true.

This is called

```
Spurious Wake-up
```

Or

Another consumer

may consume

the data first.

Therefore

Always recheck condition.

---

# 11. signal() vs broadcast()

signal()

```
Wake

ONE

Thread
```

broadcast()

```
Wake

ALL

Threads
```

Suppose

5 Consumers

waiting.

Producer

creates

ONE

item.

Use

```
signal()
```

Suppose

Program shutting down.

Wake

everyone.

Use

```
broadcast()
```

---

# 12. Producer Consumer (Buffer Size = 1)

Shared Variables

```c
int buffer;
int full=0;
```

Producer

```
while(full)

↓

Wait

↓

Produce

↓

full=1

↓

Signal
```

Consumer

```
while(!full)

↓

Wait

↓

Consume

↓

full=0

↓

Signal
```

This ensures

Producer never overwrites data.

Consumer never reads invalid data.

---

# 13. Continuous Producer & Consumer Consumes 10 Items

Suppose

Producer

produces

100 numbers.

Consumer

should consume

10 numbers

before waiting again.

Example

```
Producer

1

2

3

4

...

100
```

Consumer

```
Wake

↓

Consume

1

↓

2

↓

3

↓

...

↓

10

↓

Sleep Again
```

Instead of

```
Wake

↓

Consume

1

↓

Sleep

↓

Wake

↓

Consume

2
```

This reduces

Context Switches

and improves

Performance.

One implementation is:

Producer

```
Fill Queue

↓

Signal Once
```

Consumer

```c
while(items_available)
{
    consume();
}
```

Consumer keeps consuming until the queue becomes empty.

Only then

waits again.

This design is used in

```
Network Drivers

Work Queues

Thread Pools

Packet Processing

Linux Kernel
```

---

# 14. Common Mistakes

Mistake 1

Using

```
if

instead of

while
```

Wrong.

---

Mistake 2

Calling

```
pthread_cond_wait()

WITHOUT

mutex
```

Undefined behavior.

---

Mistake 3

Forgetting

to change

shared condition

before signaling.

Wrong

```
Signal

↓

Then

full=1
```

Correct

```
full=1

↓

Signal
```

---

Mistake 4

Thinking

Condition Variable

stores

data.

It stores

NO DATA.

It only waits

for

a condition.

---

# 15. Real World Use Cases

Linux Thread Pool

```
No Task

↓

Worker Sleeps

↓

Task Arrives

↓

Signal Worker
```

Camera Driver

```
Frame Captured

↓

Wake Processing Thread
```

Network Driver

```
Packet Received

↓

Wake Network Thread
```

Bluetooth Stack

```
Event Arrives

↓

Wake Event Thread
```

Sensor Processing

```
Temperature Ready

↓

Wake Processing Thread
```

---

# 16. Interview Questions

Q.

Why isn't mutex enough?

Answer

Because a mutex protects shared data but does not notify waiting threads when a condition becomes true.

---

Q.

Why does pthread_cond_wait()

take mutex?

Answer

It atomically

1. Unlocks mutex
2. Sleeps
3. Wakes
4. Locks mutex again

preventing deadlocks and race conditions.

---

Q.

Why while instead of if?

Answer

Because of

```
Spurious Wake-ups

and

Lost Wake-ups
```

The condition must always be rechecked.

---

Q.

Difference

Mutex

vs

Condition Variable?

Mutex

```
Protects Data
```

Condition Variable

```
Waits

for

Data
```

---

# 17. Summary

| Feature | Condition Variable |
|----------|-------------------|
| Protect Data | No |
| Wait for Event | Yes |
| Needs Mutex | Yes |
| Busy Waiting | No |
| CPU Usage | Very Low |
| Wakes One Thread | signal() |
| Wakes All Threads | broadcast() |

---

# Important Points

✓ Condition Variable does NOT replace Mutex.

✓ Mutex protects data.

✓ Condition Variable waits for state changes.

✓ pthread_cond_wait() automatically unlocks and relocks the mutex.

✓ Always use while(), never if().

✓ Condition Variables are ideal for Producer-Consumer, work queues, and event-driven programming.

---

End of Chapter 4
